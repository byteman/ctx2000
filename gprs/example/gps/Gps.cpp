/*
*****************************************Copyright (c)***********************
**                               CHONGQING DATA TECHNOLOGY
**                                    
**                                  DT800
**--------------文件信息-----------------------------------------------------
**文   件   名: gps.c
**创   建   人: 
**最后修改日期: 
**描        述: 
**              
**
**--------------历史版本信息-------------------------------------------------
** 创建人: 侯朝能
** 版  本: V0.1
** 日　期: 
** 描　述: 
**
**---------------------------------------------------------------------------
** 修改人: 侯朝能
** 版  本: 
** 日　期: 2010年5月18日20:38:45
** 描　述: 添加主控，控制GPS发送间隔，添加，地图信息更改后的地图图片坐标值
**
**--------------当前版本修订-------------------------------------------------
** 修改人: 
** 日　期: 2010年11月22日13:25:24
** 描　述: 修改报警冲突问题
**
**
******************************************************************************/
#include "Gps.h"
#include "../../msg/dt800message.h"
#include "../../msg/msgtype.h"
#include "../main/dt_common.h"
#include <sys/types.h>
#include <sys/stat.h>
#ifndef S_SPLINT_S
#include <sys/socket.h>
#include <unistd.h>
#endif /* S_SPLINT_S */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <termios.h>
#include <assert.h>
#include "gpsdclient.h"
#include <time.h>
#include <sys/types.h>
#include "gpsd_config.h"
#include <math.h>

#ifdef HAVE_SYS_SOCKET_H
#ifndef S_SPLINT_S
#include <sys/socket.h>
#endif /* S_SPLINT_S */
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN_SYSTM_H
#include <netinet/in_systm.h>
#endif
#ifndef S_SPLINT_S
#ifdef HAVE_NETINET_IN_H
#include <netinet/ip.h>
#endif /* S_SPLINT_S */
#endif
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

/* gps_open() errno return values */
#define NL_NOSERVICE    -1    /* can't get service entry */
#define NL_NOHOST       -2    /* can't get host entry */
#define NL_NOPROTO      -3    /* can't get protocol entry */
#define NL_NOSOCK       -4    /* can't create socket */
#define NL_NOSOCKOPT    -5    /* error SETSOCKOPT SO_REUSEADDR */
#define NL_NOCONNECT    -6    /* can't connect to host */

#define debug 0
#define GPS_SLEEP_TIME 10
#define GPS_SEND_POINT_CONT 4

#if !defined (INADDR_NONE)
#define INADDR_NONE   ((in_addr_t)-1)
#endif
#define GPS_OK              0
#define GPS_NO_DATA         -1
#define GPSD_OPEN           "gpsd /dev/ttymxc3"
#define GPSD_CLOSE          "killall gpsd"

#define GPS_NOSOCK          -4 /* can't create socket */
#define GPS_NOCONNECT       -6/* can't connect to host */
#define GPS_ROUTE_E         -7/* can't get lo and la */
#define GPS_SPEED_E         -8/* can't get speed */
#define GPS_Point_E         -9/* can't get local  */
#define GPS_Point_E_INIT    -10/* can't get local  */
#define GPS_ROUTE_E_INIT    -11/* rotue init error */
#define GPS_SPEED_E_INIT    -12/* speed init error */
#define GPS_RAIL_E_INIT     -13/* rail init error */
char  time_no_year[16];
static unsigned   int   dGPS_N_UP    =1;//9-12    纬度（整型化*1000000）    4字节 纬度 ddmm mmmm 
static unsigned   int   dGPS_E_up    = 2;
static unsigned   int   dGPS_N_down  =3;//9-12    纬度（整型化*1000000）    4字节 纬度 ddmm mmmm 
static unsigned   int   dGPS_E_down  = 4;

static unsigned   int   iGPS_UP_x_table = 1;
static unsigned   int   iGPS_UP_y_table = 1;
static unsigned   int   iGPS_DOWN_x_table = 5;
static unsigned   int   iGPS_DOWN_y_table = 5;

static unsigned   int   iGPS_x_table = 3;//地图图片X像素点
static unsigned   int   iGPS_y_table = 4;//地图图片Y像素点

static unsigned  int    iGPS_APP_CON_time = 1;
static unsigned  int    iGPS_GUI_CON_time = 1;
static char             gcP_UP_La_Symbols;
static char             gcP_UP_Lo_Symbols;
static char             gcP_DOWN_La_Symbols;
static char             gcP_DOWN_Lo_Symbols;
static char             cRead_buf[512];
static char             cChar_Mark_Rec_Data = 0;
static char             cGUI_Mark_Rec_Data  = 0;
pthread_mutex_t GPS_Rec_Tre;
///路线偏移
vector<GPS_Route_Point_data> GPS_vecPoint_data;
static struct GPS_Route_data GPS_Offset_A_data={0,-1,-1,false};
static  bool     bRoute_Send_Alarm = false;
///超速报警
vector<GPS_Speed_Point_data> GPS_vecS_Point;
static struct GPS_Speed_Cont_data GPS_GSpeed_Cont_data={-1,-1,-1,false};
static int      gAll_speed_max = 1;
static int      gAll_speed_count_max = 3;
static  bool    bSpeed_Send_Alarm = false;
///到点报警
vector<GPS_Point_data> gvecP_Point;
static   struct GPS_Point_Contrl gGPS_Point_Cont={0,-1,-1,false};
static bool   GPS_Route_Contrl_new = false;
static bool    bPoint_Send_Alarm = false;
//停车最大时间
static bool     bGPS_Stop_Max_time  = false;
unsigned int    uiGPS_Stop_time = 0;
//gps无信号时间
static bool     bGPS_NO_Singal_Max_time  = false;
unsigned int    uiGPS_NO_Singal_Stop_time = 0;
//电子围栏
vector<GprsRail> gvec_GprsRail;
static  bool     bRail_Send[1024] = {false};
static  bool     bRail_init_flag = false;
static 	GprsRail sGprsRail;
///@brief
///GPS报警
static  unsigned int AlarmFlagGPS = 0x00000000;
///报警锁
pthread_mutex_t mut;

static struct  gprmc //推荐定位信息
/* describe a data  source */
{
    char GPS_Latitude[16] ;         // 4字节 纬度 ddmmmmmm
    char GPS_Longitude[16];         // 4字节 经度 dddmmmmmm
    char GPS_APP_Latitude[16] ;     // 4字节 纬度 ddmmmmmm
    char GPS_APP_Longitude[16];     // 4字节 经度 dddmmmmmm
    char GPS_Height[16];            // 4字节 高度 0  
    char GPS_Speed[16];             // 4字节 速度 dddmm knote
    char GPS_GUI_Speed[16];         // 4字节 传给GUI速度 D.DDD
    char GPS_Signal_strength[16];   // 1字节 信号强度 0
    char GPS_GUI_Signal_strength[16];   // 1字节 信号强度 0
    char GPS_Latitude_sign[16];     //'N' 'S'
    char GPS_Longitude_symbol[16];  //'E' W'
    char GPS_Time[16];              //Long，4字节
    char time_hhmmss[16];
    char nianyueri[16];
    
}GPS_gprmc;
static struct  gpgga                //gps定位信息
/* describe a data  source */
{
    char GPS_stuts[16];             /* pointer to actual storage */
    char ST_num[16];                //定位状态
    char HDOP[16];
}GPS_gpgga;
/* Serial port variables */

extern dt800msg   *g_AppMsg;

/*****************************************************************************
** 函数名称 : getfirstmsg
** 功能描述 : 解析字符串
** 输   入  :  :
**          : 
**            
** 输　 出  : 0 :
**            1 :
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static int getfirstmsg(string text,string &first,string &others)
{
    int pos=text.find(',',0);
    if ((pos <= 0) || (pos+1)>=text.length()) {
        return -1;
    }
    first.assign(text,0,pos);
    others.assign(text,pos+1,text.length());
    return 0;
}
/*****************************************************************************
** 函数名称 : get_signal_msg
** 功能描述 : 解析字符串*字符串
** 输   入  :  :
**          : 
**            
** 输　 出  : 0 :
**            1 :
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static int get_signal_msg(string text,string &first,string &others)
{
    int pos=text.find('*',0);
    if ((pos <= 0) || (pos+1)>=text.length()) {
        return -1;
    }
    first.assign(text,0,pos);
    others.assign(text,pos+1,text.length());
    return 0;
}

/*****************************************************************************
** 函数名称 : GPS_F_weidu
** 功能描述 : 解析纬度值
** 输   入  :  :
**          : 
**            
** 输　 出  : 0 :
**            1 :
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

static double GPS_F_weidu(char north_south[12]){
    int dd;
    float    mmmmmm;//2943.25049  10627.3950
    double ddmmmmm;
    char cDD[4],cMM[9];

    strncpy(cDD,north_south,2);
    cDD[2] = '\0';
    dd = atoi(cDD);
    strncpy(cMM,north_south+2,8);
    cMM[9] = '\0';
    mmmmmm = atof(cMM);
    mmmmmm = mmmmmm /60;
    ddmmmmm = (dd + mmmmmm)*1000000; 
    return ddmmmmm;
}
/*****************************************************************************
** 函数名称 : jingdu
** 功能描述 : 解析经度值
** 输   入  :  :
**          : 
**            
** 输　 出  : 0 :
**            1 :
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

static double  GPS_F_jingdu(char north_south[12]){
    int dd= 0;
    float    mmmmmm;//2943.25049  10627.3950
    double ddmmmmm=0;
    char cDD[5],cMM[9];
    strncpy(cDD,north_south,3);
    cDD[3] = '\0';
    dd = atoi(cDD);
    strncpy(cMM,north_south+3,8);
    cMM[9] = '\0';
    mmmmmm = atof(cMM);
    mmmmmm = mmmmmm /60;
    ddmmmmm = (dd + mmmmmm)*1000000; 
    return ddmmmmm;
}
//处理坐标值
/*****************************************************************************
** 函数名称 : GPS_TO_GUI
** 功能描述 : 修改从任意两点得到GUI坐标值
** 输   入  :  :
**          : 
**            
** 输　 出  : 0 :
**            1 :
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 2010年6月1日
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

static int GPS_TO_GUI(char north_south[12],char est_wth[12],char weidu_log[12],char jingdu_log[12]){
    double dX,dY;
    double dM_up,dMset_down;//29432770  106273950
    float fDv_Y = 0;        //一个像素点所代表的纬度值
    float fDv_X = 0;        //一个像素点所代表的纬度值

    dM_up = GPS_F_weidu( north_south);
    dMset_down = GPS_F_jingdu( est_wth);
    //dM_up = 29726224;
    //dMset_down = 106455041;
    //GPS_PRINTF("north_south = %s\n",north_south);
   // GPS_PRINTF("dM_up  = %f\n",dM_up);

    float fY = 0;
    char cLO1 = gcP_UP_La_Symbols;
    char cLO2 = gcP_DOWN_La_Symbols;
    float fY1 = iGPS_UP_y_table;
    float fY2 = iGPS_DOWN_y_table;
    float fN1 = dGPS_N_UP;
    float fN2 = dGPS_N_down;
    if((fN1 == fN2)||(fY1== fY2)){
        return -1;
    }
    //判断赤道位置
    if(cLO1 == cLO2){
        fDv_Y =( (fN1 - fN2)/(fY1 - fY2));
    }else{
        fDv_Y =( (fN1 + fN2)/(fY1 - fY2));
    }
    if(fDv_Y < 0){
        fDv_Y = -fDv_Y;  
    }else if(fDv_Y ==0){
        return -1;
    }
    //确定Y坐标
    if((cLO1 == weidu_log[0] )){
        fY = (( fN1-dM_up)/fDv_Y)+fY1;
    }else{
        fY = (( fN1+dM_up)/fDv_Y)+fY1;
    }

    if(fY < 0){
        fY = 0;
    }else if(fY >iGPS_y_table){
        fY = iGPS_y_table;
    }

    char cLA1 = gcP_UP_Lo_Symbols;
    char cLA2 = gcP_DOWN_Lo_Symbols;
    float fX = 0;
    float fX1 = iGPS_UP_x_table;
    float fX2 = iGPS_DOWN_x_table;
    float fE1 = dGPS_E_up;
    float fE2 = dGPS_E_down;
    if((fE1 == fE2)||(fX1== fX2)){
        return -1;
    }
    //判断格林威治
    if(cLA1 == cLA2){
        fDv_X =( ( fE2-fE1)/(fX1-fX2));
    }else{
        fDv_X =( ( fE2+fE1)/( fX2 - fX1));
    }
    if(fDv_X < 0){
        fDv_X = -fDv_X;  
    }else if(fDv_X == 0){
        return -1;
    }
    //GPS_PRINTF("         fDv_X = %f\n",fDv_X);

    //确定X坐标
    if((cLA1 == jingdu_log[0])){
        fX = ((dMset_down - fE1)/fDv_X)+fY1;
    }
    else{
        fX = ((dMset_down + fE1)/fDv_X)+fY1;
    }

    if(fX<0){
        fX = 0;
    }else if(fX > iGPS_x_table){
        fX = iGPS_x_table;
    }
    sprintf(GPS_gprmc.GPS_Longitude,"%d",(unsigned int)(fX+0.5));
    sprintf(GPS_gprmc.GPS_Latitude,"%d",(unsigned int)(fY+0.5));
    
    return 0;


}

/*****************************************************************************
** 函数名称 : get_time
** 功能描述 : 得到时间值
** 输   入  :  :
**          : 
**            
** 输　 出  : 0 :
**            1 :
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static int get_time(char time_years[12],char time_hhmmss[12]){
    
    char cYear [3];
    char cMonth [3];
    char cDay [3];
    char cHour [3];
    char cMini [3];
    char cSecond [3];
    char bufdate[20];
    struct tm gps_time,*sys_time;
    time_t t_of_day,t_sys_time;
    gps_time.tm_year    =atoi(strncpy(cYear ,time_years+4,2))+100;
    gps_time.tm_mon     =atoi(strncpy(cYear ,time_years+2,2)) -1;
    gps_time.tm_mday    =atoi(strncpy(cYear ,time_years,2));
    gps_time.tm_hour    =atoi(strncpy(cYear ,time_hhmmss,2));
    gps_time.tm_min     =atoi(strncpy(cYear ,time_hhmmss+2,2));
    gps_time.tm_sec     =atoi(strncpy(cYear ,time_hhmmss+4,2));
    gps_time.tm_isdst=0;
    t_of_day=mktime(&gps_time);
    if(t_of_day < 1262347200){
        return -1;
    }
    
    t_of_day = (int)t_of_day + 28800;
    sprintf(GPS_gprmc.time_hhmmss,"%d",(int)t_of_day);
    #if 1
    sys_time = localtime(&t_of_day);
    t_of_day=mktime(sys_time);

    time(&t_sys_time);
    if(((int)t_sys_time - (int)t_of_day)>10||((int)t_sys_time - (int)t_of_day)<-10){
        sprintf(bufdate, "date %d.%d.%d-%d:%d:%d",sys_time->tm_year + 1900,sys_time->tm_mon + 1, sys_time->tm_mday,sys_time->tm_hour,sys_time->tm_min,sys_time->tm_sec);
        system(bufdate);
        system("hwclock -w");
    }
    #endif
    return 0;

}
/*****************************************************************************
** 函数名称 : get_gpgga
** 功能描述 : 解析GPS坐标信息，时间信息
** 输   入  :  :
**          : 
**            
** 输　 出  : 0 :return 0 for success, 
**            -1 :return 0 for error，
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
int get_gpvtg(string strText,string &strFirst,string &strOthers){
    char speed[16];
    char signal_strength[2]; 
    char km[16] = "0";
    char *point;
    float s;
    float fSpeed = 0;
    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }
    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }
    
    strncpy(GPS_gprmc.GPS_Speed , strFirst.c_str(),8);
    strncpy(speed, strFirst.c_str(),8);         //获取速度
    fSpeed = atof(speed);

    fSpeed = fSpeed*100;
    sprintf(GPS_gprmc.GPS_Speed,"%d",(int)fSpeed);
    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }
    char cTAG[2];
    strncpy(cTAG, strFirst.c_str(),1);
    if(cTAG[0] != 'K'){
        return -1;
    }

    if(get_signal_msg(strText, strFirst, strText) < 0){
        return -1;
    }
    
    strncpy(signal_strength, strFirst.c_str(),2);//获取信号强度
    if(signal_strength[0] == 'A'){
        signal_strength [0]='3'; 
    }
    else if(signal_strength[0]  == 'D')
    {
        signal_strength [0]='2'; 
    }
    else if(signal_strength[0]  == 'E')
    {
        signal_strength [0]='1'; 
    }
    else{
        signal_strength [0]='0'; 
    }
    #if 1
    if(true == GPS_GSpeed_Cont_data.bGPS_Speed_On_OFF)
    {

        if(GPS_GSpeed_Cont_data.uiGPS_ALL_Max_Speed >= 0){
            int iSpeed_temp = (int)(atof(speed)+0.5);
            if(iSpeed_temp > GPS_GSpeed_Cont_data.uiGPS_ALL_Max_Speed){
                gAll_speed_count_max++;
                if(gAll_speed_count_max > 3){
                    GPS_Speed_Alarm_to_APP( iSpeed_temp);
                    gAll_speed_count_max = 0;
                }
            }
            else
            {
                ///@brief
                ///取消最大超速报警
                if(true == bSpeed_Send_Alarm )
                {
                    GPSAlarmChange(A_POS_SPEED,false);
                    bSpeed_Send_Alarm = false;
                }
            }
        }
    }
    #endif
    //转化小数        
    sprintf(speed,"%d",(int)(atof(speed)+0.5));

    //增加报警信息

    strncpy(GPS_gprmc.GPS_GUI_Speed , speed,8);
    strncpy(GPS_gprmc.GPS_GUI_Signal_strength , signal_strength,8);

    return 0;

}
/*****************************************************************************
** 函数名称 : get_gpgga
** 功能描述 : 解析GPS信号信息，状态，以及卫星数目
** 输   入  :  :
**          : 
**            
** 输　 出  : 0 :return 0 for success, 
**            -1 :return 0 for error，
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

static int get_gpgga(string strText,string &strFirst,string &strOthers){
    int  singal = 0;
    char singal_b_s[2] = {0};

    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }
    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }
    strncpy(GPS_gpgga.GPS_stuts, strFirst.c_str(),2);

    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }
    strncpy(GPS_gpgga.ST_num, strFirst.c_str(),2);

    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }

    strncpy(GPS_gpgga.HDOP, strFirst.c_str(),2);
    singal = atoi(GPS_gpgga.HDOP);
     
    if(getfirstmsg(strText, strFirst, strText) < 0){ //海拔高度
        return -1;
    }
    strncpy(GPS_gprmc.GPS_Height, strFirst.c_str(),16);
    sprintf(GPS_gprmc.GPS_Height,"%ld",(long int)((atof(GPS_gprmc.GPS_Height))*1000000));


    if((0.5<=singal )&&(singal <= 15)){
        singal_b_s [0]='3'; 
    }
    else if((15<=singal )&&(singal <= 30))
    {
        singal_b_s [0]='2'; 
    }
    else if((30<=singal )&&(singal <= 50))
    {
        singal_b_s [0]='1'; 
    }
    else{
        singal_b_s [0]='0'; 
    }
    strncpy(GPS_gprmc.GPS_Signal_strength,singal_b_s,2);

    return 0;
}

/*****************************************************************************
** 函数名称 : get_gprmc
** 功能描述 : 解析GPS坐标信息，时间信息
** 输   入  :  :
**          : 
**            
** 输　 出  : 0 :return 0 for success, 
**            -1 :return 0 for error，
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

static int get_gprmc(string strText,string &strFirst,string &strOthers){

    char time_years[12] ;
    char time_hhmmss[12]="260510";
    char cGPS_Latitude[16];
    char cGPS_Longitude[16];
    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }
    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }
    strncpy(time_hhmmss, strFirst.c_str(),6);                   //获取时分秒

    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }
    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }

    strncpy(cGPS_Latitude, strFirst.c_str(),11);          //获取纬度

    strncpy(GPS_gprmc.GPS_APP_Latitude , strFirst.c_str(),11);
    
    sprintf(GPS_gprmc.GPS_APP_Latitude,"%ld",(long)(GPS_F_weidu( GPS_gprmc.GPS_APP_Latitude)));
    
    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }
    strncpy(GPS_gprmc.GPS_Latitude_sign, strFirst.c_str(),2);//获取纬度半球

    if(getfirstmsg(strText, strFirst, strText) < 0){
        return -1;
    }

    strncpy(cGPS_Longitude, strFirst.c_str(),15);         //获取经度
    strncpy(GPS_gprmc.GPS_APP_Longitude , strFirst.c_str(),15);
    sprintf(GPS_gprmc.GPS_APP_Longitude,"%ld",(long)(GPS_F_jingdu( GPS_gprmc.GPS_APP_Longitude)));

    if(getfirstmsg(strText, strFirst, strOthers) < 0){
        return -1;
    }

    strncpy(GPS_gprmc.GPS_Longitude_symbol, strFirst.c_str(),2);    //获取经度半球
    
    char *nianyueri = NULL;
    
    nianyueri = strstr(strText.c_str(),",");
    nianyueri = strstr(nianyueri+1,",");
    nianyueri = strstr(nianyueri+1,",");
    strncpy(time_years, nianyueri+1,6);                             //获取经度半球
    time_years[6]='\0';

    if(get_time(time_years,time_hhmmss) < 0){
        return -1;
    }
    //处理坐标值把錮ddmm.mmmm转换为dd.dddddd格式
    //GPS_PRINTF("GPS_gprmc.GPS_Latitude = %s\n",GPS_gprmc.GPS_Latitude);
    //usleep(10);
    if(GPS_TO_GUI(cGPS_Latitude,cGPS_Longitude,GPS_gprmc.GPS_Latitude_sign,GPS_gprmc.GPS_Longitude_symbol) < 0){
        return -1;
    }
    return 0;
}



/*****************************************************************************
** 函数名称 : gpsd_source_spec
** 功能描述 : standard parsing of a GPS data source spec
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

 void gpsd_source_spec(const char *arg, struct fixsource_t *source)
{
    source->server  = "127.0.0.1";
    //strncpy(source->server,"127.0.0.1",10);
    source->port    = DEFAULT_GPSD_PORT;
    //strncpy(source->port,"2947",5);
    source->device  = NULL;

}
/*****************************************************************************
** 函数名称 : netlib_connectsock
** 功能描述 : 
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

static int netlib_connectsock(const char *host, const char *service, const char *protocol)
{
    struct hostent *phe;
    struct servent *pse;
    struct protoent *ppe;
    struct sockaddr_in sin;
    int s, type, proto, one = 1;

    memset((char *) &sin, 0, sizeof(sin));
    /*@ -type -mustfreefresh @*/
    sin.sin_family = AF_INET;
    if ((pse = getservbyname(service, protocol)))
        sin.sin_port = htons(ntohs((unsigned short) pse->s_port));
    else if ((sin.sin_port = htons((unsigned short) atoi(service))) == 0)
        return NL_NOSERVICE;

    if ((phe = gethostbyname(host)))
        memcpy((char *) &sin.sin_addr, phe->h_addr, phe->h_length);
    else if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
        return NL_NOHOST;

        ppe = getprotobyname(protocol);
    if (strcmp(protocol, "udp") == 0) {
        type = SOCK_DGRAM;
        proto = (ppe) ? ppe->p_proto : IPPROTO_UDP;
    } else {
        type = SOCK_STREAM;
        proto = (ppe) ? ppe->p_proto : IPPROTO_TCP;
    }

    if ((s = socket(PF_INET, type, proto)) < 0)
        return NL_NOSOCK;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one))==-1) {
        (void)close(s);
        return NL_NOSOCKOPT;
    }
    if (connect(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        (void)close(s);
        return NL_NOCONNECT;
    }

#ifdef IPTOS_LOWDELAY
    {
        int opt = IPTOS_LOWDELAY;
        (void)setsockopt(s, IPPROTO_IP, IP_TOS, &opt, sizeof opt);
    }
#endif
#ifdef TCP_NODELAY
    if (type == SOCK_STREAM)
        setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &one, sizeof one);
#endif
    return s;
    /*@ +type +mustfreefresh @*/
}
/*****************************************************************************
** 函数名称 : GPS__INIT_Client
** 功能描述 : GPS初始化,建立套接字
** 输   入  :  :int *sock 
**          : 
**            
** 输　 出  : GPS_NOSOCK 没有建立sock
**          : GPS_NOCONNECT 连接失败 
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

static int GPS__INIT_Client(int *sock)
{
    ssize_t Ret_wrote = 0;
    struct fixsource_t source;
    char buf[256] = "r=1;"; 
    gpsd_source_spec(NULL, &source);
    GPS_PRINTF("connectsock GPSD \n");
    *sock = netlib_connectsock(source.server, source.port, "tcp");

    if (*sock < 0) {
        int  ret_sys = system(GPSD_OPEN);
        if((ret_sys <0)||(ret_sys == 127)){
            return GPS_NOSOCK;
        }
        *sock = netlib_connectsock(source.server, source.port, "tcp");
    }
    if(*sock < 0){
        return GPS_NOSOCK;
    }
    Ret_wrote = write(*sock, buf, strlen(buf));
    if ((ssize_t)strlen(buf) != Ret_wrote) {
        return GPS_NOCONNECT;
    }
    return 0;
}
/*****************************************************************************
** 函数名称 : GPS_Analysis_Data
** 功能描述 : 解析GPS信息，得到时间，坐标，速度，信号
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

static void GPS_Init_gprmc(void){
    strncpy(GPS_gprmc.GPS_Latitude,"ERROR",6) ;         // 4字节 纬度 ddmmmmmm
    strncpy(GPS_gprmc.GPS_Longitude,"ERROR",6);         // 4字节 经度 dddmmmmmm
    
    strncpy(GPS_gprmc.GPS_Height,"ERROR",6);            // 4字节 高度 0  
    strncpy(GPS_gprmc.GPS_APP_Latitude,"ERROR",6);      // 4字节 高度 0  
    strncpy(GPS_gprmc.GPS_APP_Longitude,"ERROR",6);     // 4字节 高度 0  


    strncpy(GPS_gprmc.GPS_GUI_Speed,"ERROR",6);         // 4字节 速度 dddmm knote
    strncpy(GPS_gprmc.GPS_GUI_Signal_strength,"0",1);   // 1字节 信号强度 0
    strncpy(GPS_gprmc.GPS_Signal_strength,"0",1);       // 1字节 信号强度 0

    strncpy(GPS_gprmc.GPS_Speed,"ERROR",6);         // 4字节 速度 dddmm knote

    strncpy(GPS_gprmc.GPS_Latitude_sign,"ERROR",6);     //'N' 'S'
    strncpy(GPS_gprmc.GPS_Longitude_symbol,"ERROR",6);  //'E' W'
    strncpy(GPS_gprmc.GPS_Time,"ERROR",6);              //Long，4字节
    strncpy(GPS_gprmc.time_hhmmss,"ERROR",6);
    strncpy(GPS_gprmc.nianyueri,"ERROR",6);

}
/*****************************************************************************
** 函数名称 : GPS_CUR_LO_LA_DATA
** 功能描述 :  获取当前的GPS信息
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
int GPS_CUR_LO_LA_DATA( GpsMapPoint& la_lo){

    //发送经度，纬度，高度，数度，信号强度，纬度符号，经度符号，时间
    //GPS_PRINTF("%s %s\n",GPS_gprmc.GPS_APP_Latitude,GPS_gprmc.GPS_APP_Longitude);
    if(!(strcmp(GPS_gprmc.GPS_APP_Latitude,"ERROR"))   ||
       !(strcmp(GPS_gprmc.GPS_APP_Longitude,"ERROR"))  ||
       !(strcmp(GPS_gprmc.GPS_Height,"ERROR"))         ||
       !(strcmp(GPS_gprmc.GPS_Latitude_sign,"ERROR"))  ||
       !(strcmp(GPS_gprmc.GPS_Longitude_symbol,"ERROR"))){
        return -1;
    }

    la_lo.latitude      =atoi(GPS_gprmc.GPS_APP_Latitude);
    la_lo.longitude     =atoi(GPS_gprmc.GPS_APP_Longitude);
    la_lo.altitude      =atoi(GPS_gprmc.GPS_Height);
    la_lo.latitude_sign =GPS_gprmc.GPS_Latitude_sign[0];
    la_lo.longitude_sign=GPS_gprmc.GPS_Longitude_symbol[0];
    return 0;
}
/*****************************************************************************
** 函数名称 : GPS_Stop_Max_time
** 功能描述 : 发送数据到app
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

int    GPS_Stop_Max_time(unsigned int time){
    if(bGPS_Stop_Max_time == true){
        if(time == uiGPS_Stop_time){
            return -1;
        }
        else 
        uiGPS_Stop_time    = time;
    }
    if(bGPS_Stop_Max_time == false){
        pthread_t     pid_Stop_Max_time;                           //数据处理线程
        uiGPS_Stop_time         = time;
        bGPS_Stop_Max_time      = true;
        pthread_create(&pid_Stop_Max_time,NULL,GPS_Stop_time,0);
    }
    return 0;
  
}
/*****************************************************************************
** 函数名称 : GPS_Stop_time
** 功能描述 : 发送数据到app
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static void *   GPS_Stop_time(void *arg){
    unsigned int    starttime       = 0, endtime = 0,curtime = 0,uiStop_time = 0;
    unsigned int    min_speed       = 100;
    char            ccurtime[12];
    bool            bNO_Speed_mark  = false;    
    uiStop_time = uiGPS_Stop_time;
    while(1){
        GETCLOCKSECOND(starttime);
        while(1){
            sleep(1);
            if(uiStop_time != uiGPS_Stop_time)
            {
                uiStop_time = uiGPS_Stop_time;
                bNO_Speed_mark = false;
            }

            ///@brief
            ///uiStop_time == 0 表示关闭停车超时报警
            if(uiStop_time == 0)
            {
                sleep(1);
                break;
            }
            if(uiStop_time != uiGPS_Stop_time){
               uiStop_time      = uiGPS_Stop_time; 
               bNO_Speed_mark   = false;    
            }
            if(!(strcmp(GPS_gprmc.GPS_Speed,"ERROR"))){
                break;
            }
            GETCLOCKSECOND(endtime);
            if(atoi(GPS_gprmc.GPS_Speed) < min_speed){
                if((endtime- starttime ) > uiStop_time){
                    if(bNO_Speed_mark == false){
                        GETCLOCKSECOND(curtime);
                        curtime = curtime - 28800;
                        sprintf(ccurtime,"%d",curtime);
                        GPSAlarmChange(A_POS_STOPTIME,true);
                        bNO_Speed_mark = true;
                    }
                    break;
                }
            }
            else{
                if(bNO_Speed_mark == true){
                    GETCLOCKSECOND(curtime);
                    curtime = curtime - 28800;
                    sprintf(ccurtime,"%d",curtime);
                    GPSAlarmChange(A_POS_STOPTIME,false);
                    bNO_Speed_mark = false;
                }

                break;
            }
        }
    }
    return 0;
}
/*****************************************************************************
** 函数名称 : GPS_No_signal_Max_time
** 功能描述 : 发送数据到app
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

int GPS_No_signal_Max_time(unsigned int time){
    if(bGPS_NO_Singal_Max_time == true){
        if(time == uiGPS_NO_Singal_Stop_time){
            return -1;
        }
        else 
        uiGPS_NO_Singal_Stop_time    = time;
    }
    if(bGPS_NO_Singal_Max_time == false){
        pthread_t     pid_NO_Signed_Max_time;                           //数据处理线程
        uiGPS_NO_Singal_Stop_time         = time;
        bGPS_NO_Singal_Max_time      = true;
        pthread_create(&pid_NO_Signed_Max_time,NULL,GPS_No_signal_time,0);
    }
    return 0;
}
/*****************************************************************************
** 函数名称 : GPS_No_signal_time
** 功能描述 : 发送数据到app
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static void *   GPS_No_signal_time(void *arg){
    unsigned int    starttime       = 0, endtime = 0,curtime = 0,uiGPS_NO_Singal_time = 0;
    char            ccurtime[12];
    bool            bNO_signal      = false;
    uiGPS_NO_Singal_time = uiGPS_NO_Singal_Stop_time;
    while(1){
        GETCLOCKSECOND(starttime);
        while(1){
            sleep(1);
            if(uiGPS_NO_Singal_time != uiGPS_NO_Singal_Stop_time){
                uiGPS_NO_Singal_time = uiGPS_NO_Singal_Stop_time;
                bNO_signal = false;
            }
            if(uiGPS_NO_Singal_time == 0){
                sleep(1);
                break;
            }
            GETCLOCKSECOND(endtime);
            if(!(strcmp(GPS_gprmc.GPS_Speed,"ERROR"))){
                if((endtime- starttime ) > uiGPS_NO_Singal_time){
                    if(bNO_signal == false){
                        GETCLOCKSECOND(curtime);
                        curtime = curtime - 28800;
                        sprintf(ccurtime,"%d",curtime);
                        GPSAlarmChange(A_POS_SIGNAL,true);
                        bNO_signal = true;
                    }
                    break;
                }
            }
            else{
                if(bNO_signal == true){
                    GETCLOCKSECOND(curtime);
                    curtime = curtime - 28800;
                    sprintf(ccurtime,"%d",curtime);
                    GPSAlarmChange(A_POS_SIGNAL,false);
                    bNO_signal = false;
                }
                break;
            }
        }
    }
    return 0;
}
/*****************************************************************************
** 函数名称 : GPS_Send_Data_APP
** 功能描述 : 发送数据到app
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static int GPS_Send_Data_APP( void){

    //发送经度，纬度，高度，数度，信号强度，纬度符号，经度符号，时间
    g_AppMsg->Send(APP_MSG,MSG_GPS_APP,string(GPS_gprmc.GPS_APP_Latitude)+MSG_SEPARATOR+GPS_gprmc.GPS_APP_Longitude+MSG_SEPARATOR+
        GPS_gprmc.GPS_Height+MSG_SEPARATOR+GPS_gprmc.GPS_Speed+MSG_SEPARATOR+GPS_gprmc.GPS_Signal_strength+MSG_SEPARATOR+GPS_gprmc.GPS_Latitude_sign
        +MSG_SEPARATOR+GPS_gprmc.GPS_Longitude_symbol+MSG_SEPARATOR+GPS_gprmc.time_hhmmss);

    return GPS_OK;
}
/*****************************************************************************
** 函数名称 : GPS_Send_Data_GUI
** 功能描述 : 解析GPS信息，得到时间，坐标，速度，信号
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static int GPS_Send_Data_GUI( void){
    //发送信号强度TO gui
    g_AppMsg->Send(GUI_MSG,MSG_GPS_SIGNAL_INTE,string(GPS_gprmc.GPS_Signal_strength));//发送信号强度消息
    //发送信号强度 to GUI
    g_AppMsg->Send(GUI_MSG,MSG_GPS_SIGNAL_STR,string(GPS_gprmc.GPS_GUI_Signal_strength));//发送信号强度消息
    g_AppMsg->Send(GUI_MSG,MSG_GPS_SIGNAL_SPEED,string(GPS_gprmc.GPS_GUI_Speed));//发送信号强度消息

    //发送图片X,Y值
    g_AppMsg->Send(GUI_MSG,MSG_GPS_SIGNAL_X_Y,string(GPS_gprmc.GPS_Longitude)+MSG_SEPARATOR+GPS_gprmc.GPS_Latitude);
    return GPS_OK;
}

/*****************************************************************************
** 函数名称 : Data_decomposition
** 功能描述 : 解析GPS信息，得到时间，坐标，速度，信号
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static int GPS_Data_decomposition(char *cBuf){

    int readbytes = 0;
    char *g_gprmc = NULL;
    char *gpgga = NULL;
    char *g_gpvtg = NULL;
    //解析时间和坐标
    //确定信号强度
    //GPS_PRINTF("gps_cBuf = %s\n",cBuf);
    gpgga=strstr(cBuf,"$GPGGA");
    if(gpgga != 0){
        char *s_gpgga = NULL;
        if(((s_gpgga = strstr(gpgga,"E")) != 0)){
            string strText(s_gpgga);
            string strFirst, strOthers;
            if(get_gpgga(strText, strFirst, strOthers) == -1){
                strncpy(GPS_gprmc.GPS_Signal_strength,"0",2);
                strncpy(GPS_gprmc.GPS_Height,"ERROR",6);


            }
        }
        else if ((s_gpgga = strstr(gpgga,"W"))!= 0){                            //判断
            string strText(s_gpgga);
            string strFirst, strOthers;
            if(get_gpgga(strText, strFirst, strOthers) == -1){
                strncpy(GPS_gprmc.GPS_Signal_strength,"0",2);
                strncpy(GPS_gprmc.GPS_Height,"ERROR",6);

            }
        }
        else{
            strncpy(GPS_gprmc.GPS_Signal_strength,"0",2);
        }
    }


    //确定速度 信号强度
    g_gpvtg=strstr(cBuf,"$GPVTG");
    if(g_gpvtg != 0){
        char *speed = NULL;
        if(((speed = strstr(g_gpvtg,"T")) != 0)){
            if(((speed = strstr(speed,"N")) != 0)){
                string strText(speed);
                string strFirst, strOthers;
                if(get_gpvtg(strText, strFirst, strOthers) == -1){
                    strncpy(GPS_gprmc.GPS_GUI_Speed,"ERROR",6);
                    strncpy(GPS_gprmc.GPS_GUI_Signal_strength,"0",6);
                    strncpy(GPS_gprmc.GPS_Speed,"ERROR",6);

                }
            }
        }
    }
    //确定坐标信息
    g_gprmc=strstr(cBuf,"$GPRMC");
    if(g_gprmc != 0){
        string strText(g_gprmc);
        string strFirst, strOthers;
        if(get_gprmc(strText, strFirst, strOthers) == -1){
            strncpy(GPS_gprmc.GPS_Longitude,"ERROR",6);
            strncpy(GPS_gprmc.GPS_Latitude,"ERROR",6);
            
            strncpy(GPS_gprmc.GPS_APP_Latitude,"ERROR",6);
            strncpy(GPS_gprmc.GPS_APP_Longitude,"ERROR",6);
            strncpy(GPS_gprmc.GPS_Latitude_sign,"ERROR",6);
            strncpy(GPS_gprmc.GPS_Longitude_symbol,"ERROR",6);
            
            strncpy(GPS_gprmc.time_hhmmss,"ERROR",6);

        }
    }
    return GPS_OK;
}
/*****************************************************************************
** 函数名称 : GPS_Analysis_Data
** 功能描述 : 解析GPS信息，得到时间，坐标，速度，信号
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static int GPS_Analysis_Data(int *sock){
    int sock_fd = 0;
    int readbytes = 0;
    char read_buf[1024];
    GPS_Init_gprmc();
    sock_fd = *sock;
    fd_set readfds;
    //bool    bGps_Module_fault  = false;
    struct timeval timeout;
    while(1){
        int count =0;
        timeout.tv_sec = GPS_SLEEP_TIME;
        timeout.tv_usec = 5000;
        FD_ZERO(&readfds);
        FD_SET(sock_fd, &readfds);
        if (select(sock_fd + 1, &readfds, NULL, NULL, &timeout) <= 0) {
            GPS_PRINTF("Line = %d |GPS|select Timeout \n",__LINE__);
            return -1;    
        }
        if (FD_ISSET(sock_fd, &readfds)) {
            memset(read_buf,'\0',sizeof(read_buf));
            readbytes = (int)read(sock_fd, read_buf, sizeof(read_buf));
            if(readbytes > 0){
                pthread_mutex_lock(&GPS_Rec_Tre);
                GPS_Data_decomposition(read_buf);
                cChar_Mark_Rec_Data = 1;
                cGUI_Mark_Rec_Data = 1;
                pthread_mutex_unlock(&GPS_Rec_Tre);
            }
            usleep(100000);
        }
    }
}
/*****************************************************************************
** 函数名称 : GPS_DATA_Receive
** 功能描述 : 得到GPS信息
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static void * GPS_DATA_Receive(void *arg)
{
    int sock = 0;
    while(1){
        GPS_PRINTF("GPS__INIT_Client\n");
        if(GPS__INIT_Client(&sock) < 0){
            GPS_PRINTF("GPS__INIT_Client error %d\n",__LINE__);
            sleep(1);
            continue;
        }
        if(GPS_Analysis_Data(&sock) < 0){
            GPS_PRINTF("GPSD_CLOSE kill gpsd\n");
            close(sock);
            int ret_sys = system(GPSD_CLOSE);
            if((ret_sys < 0)||(ret_sys == 127)){
                GPS_PRINTF("GPS__INIT_Client GPSD_CLOSE error %d\n",__LINE__);
                sleep(1);
            }
        }
    }
}
/*****************************************************************************
** 函数名称 : GPS_DATA_treatment
** 功能描述 : 得到GPS信息
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static void * GPS_DATA_to_APP_treatment(void *arg)
{
    while(1){
        if(cChar_Mark_Rec_Data == 1){
            if(iGPS_APP_CON_time == 0){
                GPS_Send_Data_APP();
                usleep(100000);
                iGPS_APP_CON_time = -1;
                while(iGPS_APP_CON_time == -1){
                    usleep(100000);
                }
                memset(cRead_buf,'\0',sizeof(cRead_buf));
                cChar_Mark_Rec_Data = 0;

            }else if(1<=iGPS_APP_CON_time&&iGPS_APP_CON_time<=65535){
                unsigned int starttime = 0, endtime = 0;
                //GPS_Data_decomposition(cRead_buf);
                GPS_Send_Data_APP();
                GETCLOCKSECOND(starttime);
                while(iGPS_APP_CON_time >= 1){
                    usleep(100000);
                    GETCLOCKSECOND(endtime);
                    if((endtime - starttime) >= iGPS_APP_CON_time){
                        cChar_Mark_Rec_Data = 0;
                        memset(cRead_buf,'\0',sizeof(cRead_buf));
                        break;
                    }
                }
                memset(cRead_buf,'\0',sizeof(cRead_buf));
                cChar_Mark_Rec_Data = 0;
            }
            cChar_Mark_Rec_Data = 0;
        }
        else{
            cChar_Mark_Rec_Data = 0;
        }
        usleep(100000);
    }
}
/*****************************************************************************
** 函数名称 : GPS_DATA_treatment
** 功能描述 : 得到GPS信息
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static void * GPS_DATA_to_GUI_treatment(void *arg)
{
    while(1){
       if(cGUI_Mark_Rec_Data == 1){

            if(iGPS_GUI_CON_time == 0){
                GPS_Send_Data_GUI();
                usleep(100000);
                iGPS_GUI_CON_time = -1;
                while(iGPS_GUI_CON_time == -1){
                    usleep(100000);
                }
                memset(cRead_buf,'\0',sizeof(cRead_buf));
                cGUI_Mark_Rec_Data = 0;

            }else if(1<=iGPS_GUI_CON_time&&iGPS_GUI_CON_time<=65535){
                unsigned int starttime = 0, endtime = 0;
                GPS_Send_Data_GUI();
                GETCLOCKSECOND(starttime);
                while(iGPS_GUI_CON_time >= 1){
                    usleep(100000);
                    GETCLOCKSECOND(endtime);
                    if((endtime - starttime) >= iGPS_GUI_CON_time){
                        cGUI_Mark_Rec_Data = 0;
                        memset(cRead_buf,'\0',sizeof(cRead_buf));
                        break;
                    }
                }
                memset(cRead_buf,'\0',sizeof(cRead_buf));
                cGUI_Mark_Rec_Data = 0;
            }
            cGUI_Mark_Rec_Data = 0;
        }
        else{
            cGUI_Mark_Rec_Data = 0;
        }
        usleep(100000);
    }
}

/*****************************************************************************
** 函数名称 : GPS_DATA_Receive
** 功能描述 : 得到GPS信息
** 输   入  :  uiaGPS_Lat_and_lon : 图片左上角经纬度，图右下角经纬度
**          :  uiGPS_Pixel_size:图片像素点大小
**          :  usGPS_TTI :主控控制GPS发送信息，0发送一次，1~65536 表示N秒发送一次  
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
int GPS_Control( struct GPS_INIT_DATA GPS_INIT_data1){
    dGPS_E_up           = GPS_INIT_data1.uiPicture_UP_Longitude;    //纬度（整型化*1000000）  4字节 纬度 ddmm mmmm 
    dGPS_N_UP           = GPS_INIT_data1.uiPicture_UP_Latitude;
    dGPS_E_down         = GPS_INIT_data1.uiPicture_DOWN_Longitude;  //纬度（整型化*1000000）  4字节 纬度 ddmm mmmm 
    dGPS_N_down         = GPS_INIT_data1.uiPicture_DOWN_Latitude;

    gcP_UP_Lo_Symbols   = GPS_INIT_data1.cPicture_UP_Longitude_Symbols;
    gcP_UP_La_Symbols   = GPS_INIT_data1.cPicture_UP_Latitude_Symbols;
    gcP_DOWN_Lo_Symbols = GPS_INIT_data1.cPicture_DOWN_Longitude_Symbols;
    gcP_DOWN_La_Symbols = GPS_INIT_data1.cPicture_DOWN_Latitude_Symbols;

    iGPS_UP_x_table     = GPS_INIT_data1.usPicture_UP_X_Pixel;
    iGPS_UP_y_table     = GPS_INIT_data1.usPicture_UP_Y_Pixel;
    iGPS_DOWN_x_table   = GPS_INIT_data1.usPicture_DOWN_X_Pixel;
    iGPS_DOWN_y_table   = GPS_INIT_data1.usPicture_DOWN_Y_Pixel;  

    iGPS_x_table        = GPS_INIT_data1.usPicture_X_Pixel;         //地图图片X像素点
    iGPS_y_table        = GPS_INIT_data1.usPicture_Y_Pixel;         //地图图片Y像素点

    iGPS_APP_CON_time   = GPS_INIT_data1.usGPS_TTI;
    iGPS_GUI_CON_time   = GPS_INIT_data1.usGPS_GUI_TTI;
    return 0;
}


/*****************************************************************************
** 函数名称 : 
** 功能描述 : 
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
int GPS_INIT()
{
    printf("%s %s %s   GPS INIT  STARTED\n",__FILE__,__DATE__,__TIME__);

    pthread_t     pid_Receive;                              //数据接收线程
    pthread_t     pid_Data_treatment;                       //数据处理线程
    pthread_t     pid_Data_gui;                             //数据处理线程
    pthread_mutex_init(&GPS_Rec_Tre, NULL);
    
    pthread_create(&pid_Receive,NULL,GPS_DATA_Receive,0);
    pthread_create(&pid_Data_treatment,NULL,GPS_DATA_to_APP_treatment,0);
    pthread_create(&pid_Data_gui,NULL,GPS_DATA_to_GUI_treatment,0);
    return 0;

}



/*****************************************************************************
** 函数名称 : GPS_Deviated_Route_init
** 功能描述 : 路线偏移初始化函数
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 2010年6月11日
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
int GPS_Deviated_Route_init(vector<GPS_Route_Point_data> vecPoint_data, struct GPS_Route_data GPS_Offset_data)
{
    bRoute_Send_Alarm   = false;
    GPS_vecPoint_data.clear();
    GPS_vecPoint_data                               = vecPoint_data;
    if(GPS_vecPoint_data.size() <= 0 ){
        GPS_PRINTF("Route Init  size = %d error!\n",GPS_vecPoint_data.size());
        return GPS_ROUTE_E_INIT;
    }
    struct GPS_Route_Point_data GPS_Route_Point_data_temp;
    for(int i = 0;i < GPS_vecPoint_data.size();i++){
        GPS_Route_Point_data_temp = GPS_vecPoint_data[i];
        //GPS_PRINTF("%10d %10d\n",GPS_Route_Point_data_temp.uiGPS_Route_Longitude,GPS_Route_Point_data_temp.uiGPS_Route_Latitude);
        if((GPS_Route_Point_data_temp.uiGPS_Route_Latitude<0)||(GPS_Route_Point_data_temp.uiGPS_Route_Latitude>90000000)){
            //GPS_PRINTF("Route Init Latitude = %d error!\n",GPS_Route_Point_data_temp.uiGPS_Route_Latitude);
            return GPS_ROUTE_E_INIT;    
        }
        if((GPS_Route_Point_data_temp.uiGPS_Route_Longitude<0)||(GPS_Route_Point_data_temp.uiGPS_Route_Longitude>180000000)){
            //GPS_PRINTF("Route Init Longitude error!\n",GPS_Route_Point_data_temp.uiGPS_Route_Longitude);
            return GPS_ROUTE_E_INIT;    
        }

    }
    GPS_Offset_A_data.uiGPS_Offset_cout             = GPS_Offset_data.uiGPS_Offset_cout;
    if((GPS_Offset_A_data.uiGPS_Offset_cout<= 0)||(GPS_Offset_A_data.uiGPS_Offset_cout >65535)){
        //GPS_PRINTF("Route Init cout = %d error!\n",GPS_Offset_A_data.uiGPS_Offset_cout);
        return GPS_ROUTE_E_INIT;
    }

    GPS_Offset_A_data.uiGPS_Offset_Cout_time        = GPS_Offset_data.uiGPS_Offset_Cout_time;
    if((GPS_Offset_A_data.uiGPS_Offset_Cout_time< 0)||(GPS_Offset_A_data.uiGPS_Offset_Cout_time >65535)){
        GPS_PRINTF("Route Init time = %d error!\n",GPS_Offset_A_data.uiGPS_Offset_Cout_time);
        return GPS_ROUTE_E_INIT;
    }
    
    GPS_Offset_A_data.uiGPS_Offset_distance         = GPS_Offset_data.uiGPS_Offset_distance;
    if((GPS_Offset_A_data.uiGPS_Offset_distance< 0)||(GPS_Offset_A_data.uiGPS_Offset_distance >65535)){
        GPS_PRINTF("Route Init distance = %d error!\n",GPS_Offset_A_data.uiGPS_Offset_distance);
        return GPS_ROUTE_E_INIT;
    }

    GPS_Offset_A_data.bOn_OFF                       = GPS_Offset_data.bOn_OFF;
    
    pthread_t     pid_GPS_Offset;                           //数据处理线程
    pthread_create(&pid_GPS_Offset,NULL,GPS_Route_Contrl,0);
    return 0;
}
/*****************************************************************************
** 函数名称 : GPS_Route_Contrl_init
** 功能描述 : 路线偏移控制函数
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 2010年6月11日
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
int GPS_Route_Contrl_init(vector<GPS_Route_Point_data> vecPoint_data, struct GPS_Route_data GPS_Offset_data)
{
    bRoute_Send_Alarm   = false;
    GPS_vecPoint_data.clear();
    GPS_vecPoint_data                               = vecPoint_data;
    if(GPS_vecPoint_data.size() <= 0 ){
        GPS_PRINTF("Route Contrl size error!\n");
        return GPS_ROUTE_E_INIT;
    }
    struct GPS_Route_Point_data GPS_Route_Point_data_temp;
    for(int i = 0;i < GPS_vecPoint_data.size();i++){
        GPS_Route_Point_data_temp = GPS_vecPoint_data[i];
        GPS_PRINTF("Route_Point    ==   %10d %10d\n",GPS_Route_Point_data_temp.uiGPS_Route_Longitude,GPS_Route_Point_data_temp.uiGPS_Route_Latitude);
        if((GPS_Route_Point_data_temp.uiGPS_Route_Latitude<0)||(GPS_Route_Point_data_temp.uiGPS_Route_Latitude>90000000)){
            GPS_PRINTF("Route Contrl Latitude error! %d\n",GPS_Route_Point_data_temp.uiGPS_Route_Latitude);
            return GPS_ROUTE_E_INIT;    
        }
        if((GPS_Route_Point_data_temp.uiGPS_Route_Longitude<0)||(GPS_Route_Point_data_temp.uiGPS_Route_Longitude>180000000)){
            GPS_PRINTF("Route Contrl Longitude = %d error! \n",GPS_Route_Point_data_temp.uiGPS_Route_Longitude);
            return GPS_ROUTE_E_INIT;    
        }

    }
    
    GPS_Offset_A_data.uiGPS_Offset_cout             = GPS_Offset_data.uiGPS_Offset_cout;
    if((GPS_Offset_A_data.uiGPS_Offset_cout<= 0)||(GPS_Offset_A_data.uiGPS_Offset_cout >65535)){
        GPS_PRINTF("Route Contrl cout = %d error!\n",GPS_Offset_A_data.uiGPS_Offset_cout);
        return GPS_ROUTE_E_INIT;
    }

    GPS_Offset_A_data.uiGPS_Offset_Cout_time        = GPS_Offset_data.uiGPS_Offset_Cout_time;
    if((GPS_Offset_A_data.uiGPS_Offset_Cout_time< 0)||(GPS_Offset_A_data.uiGPS_Offset_Cout_time >65535)){
        GPS_PRINTF("Route Contrl time = %d error!\n",GPS_Offset_A_data.uiGPS_Offset_Cout_time);
        return GPS_ROUTE_E_INIT;
    }
    
    GPS_Offset_A_data.uiGPS_Offset_distance         = GPS_Offset_data.uiGPS_Offset_distance;
    if((GPS_Offset_A_data.uiGPS_Offset_distance< 0)||(GPS_Offset_A_data.uiGPS_Offset_distance >65535)){
        GPS_PRINTF("Route Contrl distance= %d error!\n",GPS_Offset_A_data.uiGPS_Offset_distance);
        return GPS_ROUTE_E_INIT;
    }

    GPS_Offset_A_data.bOn_OFF                       = GPS_Offset_data.bOn_OFF;
    return 0;
}
/*****************************************************************************
** 函数名称 : GPS_Deviated_Route
** 功能描述 : 处理路线偏移,时间计算，开关计算次数
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static void * GPS_Route_Contrl(void *arg){
    unsigned int    starttime       = 0, endtime = 0;
    unsigned int    uiSum_count     = 1;
    unsigned int    uiCur_distance  = 0;
    //GPS_PRINTF("-------------------------route-----------------------------\n");

    if(GPS_Offset_A_data.uiGPS_Offset_cout <= 0){
        GPS_Offset_A_data.uiGPS_Offset_cout = 1;
    }
    uiSum_count = GPS_Offset_A_data.uiGPS_Offset_cout;
    GPS_PRINTF("GPS_Offset_A_data = %10d\n%10d\n%10d\n%10d\n",GPS_Offset_A_data.uiGPS_Offset_distance,GPS_Offset_A_data.uiGPS_Offset_cout,GPS_Offset_A_data.uiGPS_Offset_Cout_time,GPS_Offset_A_data.bOn_OFF);
    while(1){
        if(GPS_Offset_A_data.bOn_OFF == true){
            if(GPS_Offset_A_data.uiGPS_Offset_Cout_time == 0){
                uiCur_distance = GPS_Route_range();
                if(uiCur_distance > 0){
                    GPS_Rotue_Info_to_APP( uiCur_distance);
                }else{
                    
                }
                GPS_Offset_A_data.uiGPS_Offset_Cout_time = -1;
                while(GPS_Offset_A_data.uiGPS_Offset_Cout_time == -1){
                    usleep(100000);
                }
            }else if(1<=GPS_Offset_A_data.uiGPS_Offset_Cout_time&&GPS_Offset_A_data.uiGPS_Offset_Cout_time<=0XFFFFFFFE){
                starttime = 0, endtime = 0;
                GETCLOCKSECOND(starttime);
                uiCur_distance = GPS_Route_range();
                if(uiCur_distance > 0){
                    uiSum_count--;
                    if(uiSum_count == 0){
                        GPS_Rotue_Info_to_APP( uiCur_distance);
                        uiSum_count = GPS_Offset_A_data.uiGPS_Offset_cout;
                    }
                }else{
                    uiSum_count = GPS_Offset_A_data.uiGPS_Offset_cout;
                }
                while(GPS_Offset_A_data.uiGPS_Offset_Cout_time >= 1){
                    usleep(100000);
                    GETCLOCKSECOND(endtime);
                    if(GPS_Offset_A_data.bOn_OFF == false){
                        uiSum_count = GPS_Offset_A_data.uiGPS_Offset_cout;
                        break;
                    }else{
                        if((endtime - starttime) >= GPS_Offset_A_data.uiGPS_Offset_Cout_time){
                            break;
                        }
                    }
                }
            }
        }
        usleep(100000);
    }
    
}

/*****************************************************************************
** 函数名称 : GPS_Route_range
** 功能描述 : 处理路线偏移
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static unsigned int GPS_Route_range ()
{   
    double dStart_x,dStart_y, dEnd_x ,dEnd_y,dCur_P_x,dCur_P_y;
    unsigned int uiRet_Cur_distance = 0;
    unsigned int uiRet_Cur_dis_temp = 0;
    bool         bDis_tem           = true;
    //允许超过范围的最大距离
    bool         bAlarm_ON_OFF      = false;   
    unsigned int uiMax_distance     = GPS_Offset_A_data.uiGPS_Offset_distance;
    int          iSum_piont         = GPS_vecPoint_data.size();
    int          iCharacter_LON     = 1;
    int          iCharacter_LAT     = 1;
    //当前点转换为坐标
    if((GPS_gprmc.GPS_Longitude_symbol[0] == 'W') ||(GPS_gprmc.GPS_Longitude_symbol[0] == 'w')){
        iCharacter_LON          = -1;
    }
    else{
        iCharacter_LON          = 1;
    }
    dCur_P_x            = atof(GPS_gprmc.GPS_APP_Longitude)*iCharacter_LON; 
    //dCur_P_x            = 106450023*iCharacter_LON;  
    if((GPS_gprmc.GPS_Latitude_sign[0] == 'S') ||(GPS_gprmc.GPS_Latitude_sign[0] == 's')){
        iCharacter_LAT          = -1;
    }
    else{
        iCharacter_LAT          = 1;
    }
    dCur_P_y                    = atof(GPS_gprmc.GPS_APP_Latitude)*iCharacter_LAT; 
    //dCur_P_y                    = 29761841*iCharacter_LAT; 

    if((dCur_P_y == 0)||(dCur_P_x == 0)){
        return 0;
    }

    dCur_P_x                    = dCur_P_x*cos((dCur_P_y/1000000.0)*0.0174533)*0.111322;
    dCur_P_y                    = dCur_P_y*0.111133;
    //判断点的垂直线是否在线段上4294967296 赤道长度：40,076千米 1度111.322KM 地球的子午线总长度大约40008km。平均111.133KM
    struct GPS_Route_Point_data GPS_Rou_Point;                     //路线点
    for(unsigned int i = 0;i < GPS_vecPoint_data.size()-1;i++){
        GPS_Rou_Point               = GPS_vecPoint_data[i];
        if((GPS_Rou_Point.uiGPS_Route_lo_sign == 'W')||(GPS_Rou_Point.uiGPS_Route_lo_sign == 'w'))
            iCharacter_LON          = -1;
        else
            iCharacter_LON          = 1;
        dStart_x                    = iCharacter_LON*GPS_Rou_Point.uiGPS_Route_Longitude*cos((GPS_Rou_Point.uiGPS_Route_Latitude/1000000.0)*0.0174533)*0.111322;
        if((GPS_Rou_Point.uiGPS_Route_la_sign == 'S')||(GPS_Rou_Point.uiGPS_Route_la_sign == 's'))
            iCharacter_LAT          = -1;
        else
            iCharacter_LAT          = 1;
        dStart_y                    = iCharacter_LAT*GPS_Rou_Point.uiGPS_Route_Latitude*0.111133;
        

        GPS_Rou_Point               = GPS_vecPoint_data[i+1];
        if((GPS_Rou_Point.uiGPS_Route_lo_sign == 'E')||(GPS_Rou_Point.uiGPS_Route_lo_sign == 'e'))
            iCharacter_LON          = -1;
        else
            iCharacter_LON          = 1;

        dEnd_x                      = iCharacter_LON*GPS_Rou_Point.uiGPS_Route_Longitude*cos((GPS_Rou_Point.uiGPS_Route_Latitude/1000000.0)*0.0174533)*0.111322;
        if((GPS_Rou_Point.uiGPS_Route_la_sign == 'S')||(GPS_Rou_Point.uiGPS_Route_la_sign == 's'))
            iCharacter_LAT          = -1;
        else
            iCharacter_LAT          = 1;
        dEnd_y                      = iCharacter_LAT*GPS_Rou_Point.uiGPS_Route_Latitude*0.111133;
        
        uiRet_Cur_distance          = GPS_Route_Point_range(dStart_x,dStart_y,dEnd_x,dEnd_y,dCur_P_x,dCur_P_y); 
        //GPS_PRINTF("alarm C = %5d M = %5d\n",uiRet_Cur_distance,uiMax_distance);
        if(uiRet_Cur_distance > uiMax_distance){
            if(bDis_tem == true){
                uiRet_Cur_dis_temp = uiRet_Cur_distance;
                bDis_tem = false;     
            }
            if(uiRet_Cur_distance < uiRet_Cur_dis_temp ){
                uiRet_Cur_dis_temp = uiRet_Cur_distance;
            }
            
            bAlarm_ON_OFF           = true;   
        }else{
            bAlarm_ON_OFF           = false;
            break;
        }
    }
    
    if(bAlarm_ON_OFF == true){
        return uiRet_Cur_dis_temp;
    }else{
        if(true == bRoute_Send_Alarm)
        {
            GPSAlarmChange(A_POS_LINE,false);
            bRoute_Send_Alarm   = false;
        }
        return 0;
    }
}


/*****************************************************************************
** 函数名称 : GPS_Deviated_Route
** 功能描述 : 求点到直线的垂直线是否在线段上
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
unsigned int GPS_Route_Point_range(double dStart_X,double dStart_Y,double dEnd_X,double dEnd_Y,double dCur_P_X,double dCur_P_Y) 
{

    bool     ret_len = false;
    unsigned int uDistance = 0;
    //判断点的垂直线是否在线段上
    //计算点到线段起点的距离 
    double  dOffsetX                    = dCur_P_X - dStart_X; 
    double  dOffsetY                    = dCur_P_Y - dStart_Y; 
    double  dbSquare_Of_Distance        = pow(fabs(dOffsetX),2)+pow(fabs(dOffsetY),2); //
    double  dbPoint_To_ptStartDistance  = sqrt(dbSquare_Of_Distance); 
    //计算点到线段终点的距离 
    dOffsetX                            = dCur_P_X - dEnd_X; 
    dOffsetY                            = dCur_P_Y - dEnd_Y; 
    dbSquare_Of_Distance                = pow(fabs(dOffsetX),2) + pow(fabs(dOffsetY),2); 
    double   dbPoint_To_ptEndDistance   = sqrt(dbSquare_Of_Distance); 

    //计算直线段的长度 
    dOffsetX                            = dStart_X   -   dEnd_X; 
    dOffsetY                            = dStart_Y   -   dEnd_Y; 
    dbSquare_Of_Distance                = pow(fabs(dOffsetX),2)+pow(fabs(dOffsetY),2); 
    double   dbLine_Length              = sqrt(dbSquare_Of_Distance); 
    //如果经过点并与直线段垂直的直线和直线段的交点在直线段内，点和直线段的两个端点构成的三角形为一个非钝角 
    //三角形，极限为一个直角三角形，其中一条直边为点到直线段两个端点距离较小的那条，直角三角形的斜边为点到 
    //直线段两个端点距离较大那条，要符合条件，也就是点到直线段两个端点的距离中较大一个的平方不会不会大于其 
    //中较小的一条的平方与直线段的长度平方的和. 
    if(dbPoint_To_ptStartDistance > dbPoint_To_ptEndDistance   ) { 
        if(pow(dbPoint_To_ptStartDistance,2) >= pow(dbPoint_To_ptEndDistance,2)+pow(dbLine_Length,2)){ 
            ret_len                     = false; 
            uDistance                   = (int)dbPoint_To_ptEndDistance; 
        } 
        else 
            ret_len                     = true; 
    } 
    else{ 
        if(pow(dbPoint_To_ptEndDistance,2) >= pow(dbPoint_To_ptStartDistance,2)+pow(dbLine_Length,2)){ 
            ret_len                     = false; 
            uDistance                   = (int)dbPoint_To_ptStartDistance; 
        } 
        else 
            ret_len                     = true;
    } 

    if(ret_len == true){
        //点到直线的距离 
        if(dEnd_Y==dStart_Y){ 
            uDistance                   = (int)fabs(dCur_P_Y-dStart_Y); 
        } 
        else if(dEnd_X == dStart_X){ 
            uDistance                   = (int)fabs(dCur_P_X-dStart_X); 
        }
        else{ 
            //经过点并与X-Axis平行的直线与直线段所在直线相交，点到相交点的距离乘上相交角的正弦 
            double A = (dStart_Y - dEnd_Y);
            double B = (dEnd_X - dStart_X);
            double C = dStart_X*dEnd_Y-dEnd_X*dStart_Y;
            //   水平距离 
            uDistance = (int)(fabs((A*dCur_P_X + B*dCur_P_Y + C)/sqrt(pow(A,2)+pow(B,2))) + 0.5);
        } 
    }
    return uDistance;

}
/*****************************************************************************
** 函数名称 : GPS_Send_Data_APP
** 功能描述 : 发送数据到app
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static int GPS_Rotue_Info_to_APP(unsigned int  uiDistance){
    char sDistance[12];
    sprintf(sDistance,"%d",uiDistance);
    //发送经度，纬度，高度，数度，信号强度，纬度符号，经度符号，时间
    if(    bRoute_Send_Alarm   == false)
    {
         GPSAlarmChange(A_POS_LINE,true);
        bRoute_Send_Alarm   = true;
    }
    return GPS_OK;
}
/*****************************************************************************
** 函数名称 : GPS_Speed_Limit_init
** 功能描述 : 速度线段初始化
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 2010年6月11日
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

int GPS_Speed_Limit_init(vector<GPS_Speed_Point_data> vecS_Point,struct GPS_Speed_Cont_data GPS_Speed_Cont){
    bSpeed_Send_Alarm = false;
    GPS_vecS_Point.clear();
    GPS_vecS_Point                                  = vecS_Point;
    
    GPS_GSpeed_Cont_data.uiGPS_ALL_Max_Speed        = GPS_Speed_Cont.uiGPS_ALL_Max_Speed;
    if((GPS_GSpeed_Cont_data.uiGPS_ALL_Max_Speed < 0)||(GPS_GSpeed_Cont_data.uiGPS_ALL_Max_Speed >65535)){
        
        GPS_PRINTF("Speed Initialization error!\n");
        return GPS_ROUTE_E_INIT;
    }
    GPS_GSpeed_Cont_data.bGPS_Speed_On_OFF          = GPS_Speed_Cont.bGPS_Speed_On_OFF;

    GPS_GSpeed_Cont_data.uiGPS_Speed_cout           = GPS_Speed_Cont.uiGPS_Speed_cout;
    if((GPS_GSpeed_Cont_data.uiGPS_Speed_cout <= 0)||(GPS_GSpeed_Cont_data.uiGPS_Speed_cout >65535)){
        GPS_PRINTF("Speed Initialization error!\n");
        return GPS_ROUTE_E_INIT;
    }

    GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time      = GPS_Speed_Cont.uiGPS_Speed_Cout_time;
    if((GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time <= 0)||(GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time >65535)){
        GPS_PRINTF("Speed Initialization error!\n");
        return GPS_ROUTE_E_INIT;
    }


    if(GPS_vecS_Point.size() <= 0 ){
        GPS_PRINTF("Speed Initialization error!\n");
        return GPS_SPEED_E_INIT;
    }
    struct GPS_Speed_Point_data GPS_Speed_Point_data_temp;
    for(int i = 0;i < GPS_vecS_Point.size();i++){
        GPS_Speed_Point_data_temp = GPS_vecS_Point[i];
        //GPS_PRINTF("Speed_Point start  =  %10d %10d\n",GPS_Speed_Point_data_temp.start.uiGPS_Route_Longitude,GPS_Speed_Point_data_temp.start.uiGPS_Route_Latitude);
        //GPS_PRINTF("Speed_Point end %10d %10d\n",GPS_Speed_Point_data_temp.end.uiGPS_Route_Latitude,GPS_Speed_Point_data_temp.end.uiGPS_Route_Longitude);
        if((GPS_Speed_Point_data_temp.start.uiGPS_Route_Latitude<0)||(GPS_Speed_Point_data_temp.start.uiGPS_Route_Latitude>90000000)){
            GPS_PRINTF("Speed Init start.uiGPS_Route_Latitude = %d error!\n",GPS_Speed_Point_data_temp.start.uiGPS_Route_Latitude);
            return GPS_ROUTE_E_INIT;    
        }
        if((GPS_Speed_Point_data_temp.start.uiGPS_Route_Longitude<0)||(GPS_Speed_Point_data_temp.start.uiGPS_Route_Longitude>180000000)){
            GPS_PRINTF("Speed Init start.uiGPS_Route_Longitude = %d error!\n",GPS_Speed_Point_data_temp.start.uiGPS_Route_Longitude);
            return GPS_ROUTE_E_INIT;    
        }
        if((GPS_Speed_Point_data_temp.end.uiGPS_Route_Latitude<0)||(GPS_Speed_Point_data_temp.end.uiGPS_Route_Latitude>90000000)){
            GPS_PRINTF("Speed Init end.uiGPS_Route_Latitude = %d error!\n",GPS_Speed_Point_data_temp.end.uiGPS_Route_Latitude);
            return GPS_ROUTE_E_INIT;    
        }
        if((GPS_Speed_Point_data_temp.end.uiGPS_Route_Longitude<0)||(GPS_Speed_Point_data_temp.end.uiGPS_Route_Longitude>180000000)){
            GPS_PRINTF("Speed Init end.uiGPS_Route_Longitude = %d error!\n",GPS_Speed_Point_data_temp.end.uiGPS_Route_Longitude);
            return GPS_ROUTE_E_INIT;    
        }
    }
    pthread_t     pid_GPS_Speed_OUT;                           //数据处理线程
    pthread_create(&pid_GPS_Speed_OUT,NULL,GPS_Spped_Limit,0);
    return 0;
}
/*****************************************************************************
** 函数名称 : GPS_Speed_Contrl_init
** 功能描述 : 处理超速计算,时间计算，开关计算次数二次赋值
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 2010年6月11日
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

int GPS_Speed_Contrl_init(vector<GPS_Speed_Point_data> vecS_Point,struct GPS_Speed_Cont_data GPS_Speed_Cont){
    bSpeed_Send_Alarm = false;
    GPS_vecS_Point.clear();
    GPS_vecS_Point                                  = vecS_Point;
    GPS_GSpeed_Cont_data.uiGPS_ALL_Max_Speed        = GPS_Speed_Cont.uiGPS_ALL_Max_Speed;
    if((GPS_GSpeed_Cont_data.uiGPS_ALL_Max_Speed < 0)||(GPS_GSpeed_Cont_data.uiGPS_ALL_Max_Speed >65535)){
        GPS_PRINTF("Speed Init Max_Speed error!\n");
        return GPS_ROUTE_E_INIT;
    }
    GPS_GSpeed_Cont_data.bGPS_Speed_On_OFF          = GPS_Speed_Cont.bGPS_Speed_On_OFF;


    GPS_GSpeed_Cont_data.uiGPS_Speed_cout           = GPS_Speed_Cont.uiGPS_Speed_cout;
    if((GPS_GSpeed_Cont_data.uiGPS_Speed_cout <= 0)||(GPS_GSpeed_Cont_data.uiGPS_Speed_cout >65535)){
        GPS_PRINTF("Speed Init cout error!\n");
        return GPS_ROUTE_E_INIT;
    }

    GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time      = GPS_Speed_Cont.uiGPS_Speed_Cout_time;
    if((GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time <= 0)||(GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time >65535)){
        GPS_PRINTF("Speed Init time error!\n");
        return GPS_ROUTE_E_INIT;
    }

    if(GPS_vecS_Point.size() <= 0 ){
        GPS_PRINTF("Speed Init size = %d error!\n",GPS_vecS_Point.size());
        return GPS_SPEED_E_INIT;
    }
    
    struct GPS_Speed_Point_data GPS_Speed_Point_data_temp;
    for(int i = 0;i < GPS_vecS_Point.size();i++){
        GPS_Speed_Point_data_temp = GPS_vecS_Point[i];
        GPS_PRINTF("Speed_Point start  =  %10d %10d\n",GPS_Speed_Point_data_temp.start.uiGPS_Route_Longitude,GPS_Speed_Point_data_temp.start.uiGPS_Route_Latitude);
        GPS_PRINTF("Speed_Point end %10d %10d\n",GPS_Speed_Point_data_temp.end.uiGPS_Route_Latitude,GPS_Speed_Point_data_temp.end.uiGPS_Route_Longitude);
        if((GPS_Speed_Point_data_temp.start.uiGPS_Route_Latitude<0)||(GPS_Speed_Point_data_temp.start.uiGPS_Route_Latitude>90000000)){
            GPS_PRINTF("Speed Init Latitude error!\n");
            return GPS_ROUTE_E_INIT;    
        }
        if((GPS_Speed_Point_data_temp.start.uiGPS_Route_Longitude<0)||(GPS_Speed_Point_data_temp.start.uiGPS_Route_Longitude>180000000)){
            GPS_PRINTF("Speed Init Longitude error!\n");
            return GPS_ROUTE_E_INIT;    
        }
        if((GPS_Speed_Point_data_temp.end.uiGPS_Route_Latitude<0)||(GPS_Speed_Point_data_temp.end.uiGPS_Route_Latitude>90000000)){
            GPS_PRINTF("Speed Init Latitude error!\n");
            return GPS_ROUTE_E_INIT;    
        }
        if((GPS_Speed_Point_data_temp.end.uiGPS_Route_Longitude<0)||(GPS_Speed_Point_data_temp.end.uiGPS_Route_Longitude>180000000)){
            GPS_PRINTF("Speed Init Longitude error!\n");
            return GPS_ROUTE_E_INIT;    
        }
    }
    return 0;
}
/*****************************************************************************
** 函数名称 : GPS_Spped_Limit
** 功能描述 : 处理超速计算,时间计算，开关计算次数
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 2010年6月11日
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static void * GPS_Spped_Limit(void *arg){
    unsigned int    starttime       = 0, endtime = 0;
    unsigned int    uiSum_count     = 0;
    int             uiCur_Speed     = 0;
    uiSum_count = GPS_GSpeed_Cont_data.uiGPS_Speed_cout;
    GPS_PRINTF("speed = %10d\n%10d\n%10d\n%10d\n",GPS_GSpeed_Cont_data.uiGPS_Speed_cout,GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time,GPS_GSpeed_Cont_data.uiGPS_ALL_Max_Speed,GPS_GSpeed_Cont_data.bGPS_Speed_On_OFF);    while(1){
        if(GPS_GSpeed_Cont_data.bGPS_Speed_On_OFF == true){
            if(GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time == 0){
                uiCur_Speed = GPS_Speed_range();
                if(uiCur_Speed > 0){
                    GPS_Speed_Alarm_to_APP( uiCur_Speed);
                }else{
                    
                }
                GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time = -1;
                while(GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time == -1){
                    usleep(100000);
                }
            }else if(1<=GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time&&GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time<=65535){
                starttime = 0, endtime = 0;
                GETCLOCKSECOND(starttime);
                uiCur_Speed = GPS_Speed_range();
                if(uiCur_Speed > 0){
                    uiSum_count--;
                    if(uiSum_count == 0){
                        GPS_Speed_Alarm_to_APP( uiCur_Speed);
                        uiSum_count = GPS_GSpeed_Cont_data.uiGPS_Speed_cout;
                    }
                }else{
                    uiSum_count = GPS_GSpeed_Cont_data.uiGPS_Speed_cout;
                }
                while(GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time >= 1){
                    usleep(100000);
                    GETCLOCKSECOND(endtime);
                    if(GPS_GSpeed_Cont_data.bGPS_Speed_On_OFF == false){
                        uiSum_count = GPS_GSpeed_Cont_data.uiGPS_Speed_cout;
                        break;    
                    }else{
                        if((endtime - starttime) >= GPS_GSpeed_Cont_data.uiGPS_Speed_Cout_time){
                            break;
                        }
                    }
                }
            }
        }
        usleep(100000);
    }
}
/*****************************************************************************
** 函数名称 : GPS_Speed_range
** 功能描述 : 处理路线偏移
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static int GPS_Speed_range ()
{   
    double dStart_x,dStart_y, dEnd_x ,dEnd_y,dCur_P_x,dCur_P_y,dCur_L_s;
    unsigned int uiRet_Cur_distance = 0;
    unsigned int uiRet_Cur_dist_temp= 0;
    bool         bGPS_Mark          = true;
    int          iGPS_Speed         = (int)((atoi(GPS_gprmc.GPS_Speed)/100.0)+0.5);
    bool         bAlarm_ON_OFF      = false;   
    int          iSum_piont         = GPS_vecS_Point.size();
    int          iSection_Num       = 0;
    int          iSection_Speed     = 0;
    int          iCharacter_LON     = 1;
    int          iCharacter_LAT     = 1;
    //
  
    //当前点转换为坐标
    if((GPS_gprmc.GPS_Longitude_symbol[0] == 'W') ||(GPS_gprmc.GPS_Longitude_symbol[0] == 'w')){
        iCharacter_LON          = -1;
    }else
        iCharacter_LON          = 1;
    dCur_P_x                    = atof(GPS_gprmc.GPS_APP_Longitude)*iCharacter_LON; 
    //dCur_P_x                    = (106457738 - 10000*(rand()/(RAND_MAX+1.0)) )*iCharacter_LON; 
    if((GPS_gprmc.GPS_Latitude_sign[0] == 'S') ||(GPS_gprmc.GPS_Latitude_sign[0] == 's')){
        iCharacter_LAT          = -1;
    }else
        iCharacter_LAT          = 1;
    dCur_P_y                    = atof(GPS_gprmc.GPS_APP_Latitude)*iCharacter_LAT; 
    //dCur_P_y                    = (29764365- 10000*(rand()/(RAND_MAX+1.0)) )*iCharacter_LAT; 
    //iGPS_Speed = (int)(100 - 100*(rand()/(RAND_MAX+1.0)));
    //GPS_PRINTF("dCur_P_y = %f  dCur_P_x = %f  Speed = %s\n",dCur_P_x,dCur_P_y,GPS_gprmc.GPS_Speed);
    
    if((dCur_P_y == 0)||(dCur_P_x == 0)||(iGPS_Speed == 0)){
        return GPS_SPEED_E;
    }
    //当前的速度超过全程最大速度报警
    if(iGPS_Speed > GPS_GSpeed_Cont_data.uiGPS_ALL_Max_Speed){
        GPS_PRINTF("iGPS_Speed = %d > GPS_GSpeed_Cont_data.uiGPS_ALL_Max_Speed\n",iGPS_Speed );
        return GPS_SPEED_E;
    }
    
    dCur_P_x                    = dCur_P_x*cos((dCur_P_y/1000000.0)*0.0174533)*0.111322;
    dCur_P_y                    = dCur_P_y*0.111133;
    //线路点
    //判断点的垂直线是否在线段上4294967296 赤道长度：40,076千米 1度111.322KM 地球的子午线总长度大约40008km。平均111.133KM
    struct GPS_Speed_Point_data GPS_Speed_Line_data;                     //路线点
    for(int i = 0;i < iSum_piont;i++){
        GPS_Speed_Line_data         = GPS_vecS_Point[i];
        if((GPS_Speed_Line_data.start.uiGPS_Route_lo_sign == 'W')||(GPS_Speed_Line_data.start.uiGPS_Route_lo_sign == 'w'))
            iCharacter_LON          = -1;
        else
            iCharacter_LON          = 1;
        dStart_x                    = iCharacter_LON*GPS_Speed_Line_data.start.uiGPS_Route_Longitude*cos((GPS_Speed_Line_data.start.uiGPS_Route_Latitude/1000000.0)*0.0174533)*0.111322;

        if((GPS_Speed_Line_data.start.uiGPS_Route_la_sign == 'S')||(GPS_Speed_Line_data.start.uiGPS_Route_la_sign == 's'))
            iCharacter_LAT          = -1;
        else
            iCharacter_LAT          = 1;
        dStart_y                    = iCharacter_LAT*GPS_Speed_Line_data.start.uiGPS_Route_Latitude*0.111133;
        

        if((GPS_Speed_Line_data.end.uiGPS_Route_lo_sign == 'W')||(GPS_Speed_Line_data.end.uiGPS_Route_lo_sign == 'w'))
            iCharacter_LON          = -1;
        else
            iCharacter_LON          = 1;
        dEnd_x                      = iCharacter_LON*GPS_Speed_Line_data.end.uiGPS_Route_Longitude*cos((GPS_Speed_Line_data.end.uiGPS_Route_Latitude/1000000.0)*0.0174533)*0.111322;

        if((GPS_Speed_Line_data.end.uiGPS_Route_la_sign == 'S')||(GPS_Speed_Line_data.end.uiGPS_Route_la_sign == 's'))
            iCharacter_LAT          = -1;
        else
            iCharacter_LAT          = 1;
        dEnd_y                      = iCharacter_LAT*GPS_Speed_Line_data.end.uiGPS_Route_Latitude*0.111133;
        uiRet_Cur_distance          = GPS_Line_Speed_range(dStart_x,dStart_y,dEnd_x,dEnd_y,dCur_P_x,dCur_P_y); 
        //GPS_PRINTF(" %10f %10f %10f %10f %10f %10f\n",dStart_x,dStart_y,dEnd_x,dEnd_y,dCur_P_x,dCur_P_y);
        //GPS_PRINTF("uiRet_Cur_dist_temp = %5x\n",uiRet_Cur_distance);
        GPS_PRINTF("uiRet_Cur_dist_temp = %5d iGPS_Speed = %5d GPS_Speed_Line_data.maxspeed =%d\n",uiRet_Cur_dist_temp,iGPS_Speed,GPS_Speed_Line_data.maxspeed);           
        if(uiRet_Cur_distance != 0xFFFFFFFF){//表示当前点的位置到线段的垂直线的焦点在线段内
            if(bGPS_Mark == true){
                uiRet_Cur_dist_temp = uiRet_Cur_distance;   
            }
            bGPS_Mark               = false;  
            if(uiRet_Cur_dist_temp  >= uiRet_Cur_distance){
                uiRet_Cur_dist_temp = uiRet_Cur_distance;
                if(iGPS_Speed > GPS_Speed_Line_data.maxspeed){
                    bAlarm_ON_OFF   = true;
                    //GPS_PRINTF("uiRet_Cur_dist_temp = %5d iGPS_Speed = %5d GPS_Speed_Line_data.maxspeed =%d\n",uiRet_Cur_dist_temp,iGPS_Speed,GPS_Speed_Line_data.maxspeed);           
                }
                else{
                    bAlarm_ON_OFF   = false;
                }
            }
        }        
    }
    //GPS_PRINTF("\n\n");

    if(bAlarm_ON_OFF == true){
        //GPS_PRINTF("alarm C = %5d\n",uiRet_Cur_distance);
        //GPS_PRINTF("uiRet_Cur_dist_temp = %5d iGPS_Speed = %5d \n",uiRet_Cur_dist_temp,iGPS_Speed);           

        return iGPS_Speed;
    }else{
        if(true == bSpeed_Send_Alarm )
        {
             GPSAlarmChange(A_POS_SPEED,false);
            bSpeed_Send_Alarm = false;
        }
        return GPS_SPEED_E;
    }
}


/*****************************************************************************
** 函数名称 : GPS_Deviated_Route
** 功能描述 : 求点到直线的垂直线是否在线段上
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
unsigned int GPS_Line_Speed_range(double dStart_X,double dStart_Y,double dEnd_X,double dEnd_Y,double dCur_P_X,double dCur_P_Y) 
{
    bool     ret_len = false;
    unsigned int uDistance = 0;
    //判断点的垂直线是否在线段上
    //计算点到线段起点的距离 
    double  dOffsetX                    = dCur_P_X - dStart_X; 
    double  dOffsetY                    = dCur_P_Y - dStart_Y; 
    double  dbSquare_Of_Distance        = pow(fabs(dOffsetX),2)+pow(fabs(dOffsetY),2); //
    double  dbPoint_To_ptStartDistance  = sqrt(dbSquare_Of_Distance); 
    //计算点到线段终点的距离 
    dOffsetX                            = dCur_P_X - dEnd_X; 
    dOffsetY                            = dCur_P_Y - dEnd_Y; 
    dbSquare_Of_Distance                = pow(fabs(dOffsetX),2) + pow(fabs(dOffsetY),2); 
    double   dbPoint_To_ptEndDistance   = sqrt(dbSquare_Of_Distance); 

    //计算直线段的长度 
    dOffsetX                            = dStart_X   -   dEnd_X; 
    dOffsetY                            = dStart_Y   -   dEnd_Y; 
    dbSquare_Of_Distance                = pow(fabs(dOffsetX),2)+pow(fabs(dOffsetY),2); 
    double   dbLine_Length              = sqrt(dbSquare_Of_Distance); 
    //如果经过点并与直线段垂直的直线和直线段的交点在直线段内，点和直线段的两个端点构成的三角形为一个非钝角 
    //三角形，极限为一个直角三角形，其中一条直边为点到直线段两个端点距离较小的那条，直角三角形的斜边为点到 
    //直线段两个端点距离较大那条，要符合条件，也就是点到直线段两个端点的距离中较大一个的平方不会不会大于其 
    //中较小的一条的平方与直线段的长度平方的和. 
    if(dbPoint_To_ptStartDistance > dbPoint_To_ptEndDistance   ) { 
        if(pow(dbPoint_To_ptStartDistance,2) >= pow(dbPoint_To_ptEndDistance,2)+pow(dbLine_Length,2)){ 
            ret_len                     = false; 
            uDistance                   = (int)dbPoint_To_ptEndDistance; 
        } 
        else 
            ret_len                     = true; 
    } 
    else{ 
        if(pow(dbPoint_To_ptEndDistance,2) >= pow(dbPoint_To_ptStartDistance,2)+pow(dbLine_Length,2)){ 
            ret_len                     = false; 
            uDistance                   = (int)dbPoint_To_ptStartDistance; 
        } 
        else 
            ret_len                     = true;
    } 

    if(ret_len == true){
        //点到直线的距离 
        if(dEnd_Y==dStart_Y){ 
            uDistance                   = (int)fabs(dCur_P_Y-dStart_Y); 
        } 
        else if(dEnd_X == dStart_X){ 
            uDistance                   = (int)fabs(dCur_P_X-dStart_X); 
        }
        else{ 
            //经过点并与X-Axis平行的直线与直线段所在直线相交，点到相交点的距离乘上相交角的正弦 
            double A = (dStart_Y - dEnd_Y);
            double B = (dEnd_X - dStart_X);
            double C = dStart_X*dEnd_Y-dEnd_X*dStart_Y;
            //水平距离 
            uDistance = (int)fabs((A*dCur_P_X + B*dCur_P_Y + C)/sqrt(pow(A,2)+pow(B,2)));
            //GPS_PRINTF("uDistance = %10d\n",uDistance);
            return uDistance;
        } 
    }
    return 0xFFFFFFFF;

}
/*****************************************************************************
** 函数名称 : GPS_Send_Data_APP
** 功能描述 : 发送数据到app
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static int GPS_Speed_Alarm_to_APP( int  iSpeed){
    char sSpeed[12];
    sprintf(sSpeed,"%d",iSpeed);
    //发送经度，纬度，高度，数度，信号强度，纬度符号，经度符号，时间
    if(false == bSpeed_Send_Alarm )
    {
        GPSAlarmChange(A_POS_SPEED,true);
        bSpeed_Send_Alarm = true;
    }
    return GPS_OK;
}
/*****************************************************************************
** 函数名称 : GPS_Piont_init
** 功能描述 : 到点信息报警
** 输   入  :  :vecP_Point 输入坐标值 最大数为65535
**          :  :GPS_Point_Con 到点报警控制参数
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 2010年6月17日
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
int  GPS_Point_init(vector<GPS_Point_data> vecP_Point,struct GPS_Point_Contrl GPS_Point_Con){
    gvecP_Point.clear();
    gvecP_Point                                 = vecP_Point;
    if(gvecP_Point.size() <= 0 ){
        GPS_PRINTF("Point Init size = %d error!\n",gvecP_Point.size());
        return GPS_Point_E_INIT;
    }
    
    struct GPS_Point_data GPS_Point_data_temp;
    for(int i = 0;i < gvecP_Point.size();i++){
        GPS_Point_data_temp = gvecP_Point[i];
        GPS_PRINTF("%10d %10d\n",GPS_Point_data_temp.uiGPS_Point_Latitude,GPS_Point_data_temp.uiGPS_Point_Longitude);
        if((GPS_Point_data_temp.uiGPS_Point_Latitude<0)||(GPS_Point_data_temp.uiGPS_Point_Latitude>90000000)){
            GPS_PRINTF("Point Init Latitude= %d error!\n",GPS_Point_data_temp.uiGPS_Point_Latitude);
            return GPS_Point_E_INIT;    
        }
        if((GPS_Point_data_temp.uiGPS_Point_Longitude<0)||(GPS_Point_data_temp.uiGPS_Point_Longitude>180000000)){
            GPS_PRINTF("Point Init Longitude = %d error!\n",GPS_Point_data_temp.uiGPS_Point_Longitude);
            return GPS_Point_E_INIT;    
        }
    }

    
    gGPS_Point_Cont.uiGPS_Point_distance        = GPS_Point_Con.uiGPS_Point_distance;
    if((gGPS_Point_Cont.uiGPS_Point_distance<0)||(gGPS_Point_Cont.uiGPS_Point_distance >65535)){
        GPS_PRINTF("Point Init distance = %d error!\n",gGPS_Point_Cont.uiGPS_Point_distance);
        return GPS_Point_E_INIT;
    }
    
    gGPS_Point_Cont.uiGPS_Point_cout            = GPS_Point_Con.uiGPS_Point_cout;
    if((gGPS_Point_Cont.uiGPS_Point_cout<= 0)||(gGPS_Point_Cont.uiGPS_Point_cout >65535)){
        GPS_PRINTF("Point Init cout = %d error!\n",gGPS_Point_Cont.uiGPS_Point_cout);
        return GPS_Point_E_INIT;
    }

    gGPS_Point_Cont.uiGPS_Point_Cout_time       = GPS_Point_Con.uiGPS_Point_Cout_time;
    if((gGPS_Point_Cont.uiGPS_Point_Cout_time<0)||(gGPS_Point_Cont.uiGPS_Point_Cout_time >65535)){
        GPS_PRINTF("Point Init = %d error!\n",gGPS_Point_Cont.uiGPS_Point_Cout_time);
        return GPS_Point_E_INIT;
    }
    gGPS_Point_Cont.bGPS_Point_On_OFF           = GPS_Point_Con.bGPS_Point_On_OFF;
    pthread_t     pid_GPS_Point_OUT;                           //数据处理线程
    pthread_create(&pid_GPS_Point_OUT,NULL,GPS_Point_Limit,0);
    return 0;
}
/*****************************************************************************
** 函数名称 : GPS_Piont_Contrl_init
** 功能描述 : 处理到点计算,时间计算，开关计算次数的二次赋值
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 2010年6月11日
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

int GPS_Point_Contrl_init(vector<GPS_Point_data> vecP_Point,struct GPS_Point_Contrl GPS_Point_Con){

    gvecP_Point.clear();
    gvecP_Point                                 = vecP_Point;
    if(gvecP_Point.size() <= 0 ){
        GPS_PRINTF("Point Init size = %d error!\n",gvecP_Point.size());
        return GPS_Point_E_INIT;
    }
    
    struct GPS_Point_data GPS_Point_data_temp;
    for(int i = 0;i < gvecP_Point.size();i++){
        GPS_Point_data_temp = gvecP_Point[i];
        if((GPS_Point_data_temp.uiGPS_Point_Latitude<0)||(GPS_Point_data_temp.uiGPS_Point_Latitude>90000000)){
            GPS_PRINTF("Point Init Latitude= %d error!\n",GPS_Point_data_temp.uiGPS_Point_Latitude);
            return GPS_Point_E_INIT;    
        }
        if((GPS_Point_data_temp.uiGPS_Point_Longitude<0)||(GPS_Point_data_temp.uiGPS_Point_Longitude>180000000)){
            GPS_PRINTF("Point Init Longitude = %d error!\n",GPS_Point_data_temp.uiGPS_Point_Longitude);
            return GPS_Point_E_INIT;    
        }
    }

    
    gGPS_Point_Cont.uiGPS_Point_distance        = GPS_Point_Con.uiGPS_Point_distance;
    if((gGPS_Point_Cont.uiGPS_Point_distance<0)||(gGPS_Point_Cont.uiGPS_Point_distance >65535)){
        GPS_PRINTF("Point Init distance = %d error!\n",gGPS_Point_Cont.uiGPS_Point_distance);
        return GPS_Point_E_INIT;
    }
    
    gGPS_Point_Cont.uiGPS_Point_cout            = GPS_Point_Con.uiGPS_Point_cout;
    if((gGPS_Point_Cont.uiGPS_Point_cout<= 0)||(gGPS_Point_Cont.uiGPS_Point_cout >65535)){
        GPS_PRINTF("Point Init cout = %d error!\n",gGPS_Point_Cont.uiGPS_Point_cout);
        return GPS_Point_E_INIT;
    }

    gGPS_Point_Cont.uiGPS_Point_Cout_time       = GPS_Point_Con.uiGPS_Point_Cout_time;
    if((gGPS_Point_Cont.uiGPS_Point_Cout_time<0)||(gGPS_Point_Cont.uiGPS_Point_Cout_time >65535)){
        GPS_PRINTF("Point Init = %d error!\n",gGPS_Point_Cont.uiGPS_Point_Cout_time);
        return GPS_Point_E_INIT;
    }
    gGPS_Point_Cont.bGPS_Point_On_OFF           = GPS_Point_Con.bGPS_Point_On_OFF;
    return 0;
}
/*****************************************************************************
** 函数名称 : GPS_Piont_Limit
** 功能描述 : 处理到点时间计算，开关计算次数
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/

static void *   GPS_Point_Limit(void *arg){
    unsigned int    starttime       = 0, endtime = 0;
    unsigned int    uiSum_count     = 1;
    int             uiCur_Point_dis = 0;
    int             iPoint_mark     = -1;
    bool            iSend_mark      = false;
    bool            bPoint_mark     = false;
    bool            bPoint_remark   = false;
    uiSum_count = gGPS_Point_Cont.uiGPS_Point_cout;
    while(1){
        if(gGPS_Point_Cont.bGPS_Point_On_OFF == true){
            if(gGPS_Point_Cont.uiGPS_Point_Cout_time == 0){
                uiCur_Point_dis = GPS_Point_Judge();
                if(uiCur_Point_dis >= 0){
                    GPS_Point_Info_to_APP( uiCur_Point_dis);
                }else{
                    
                }
                gGPS_Point_Cont.uiGPS_Point_Cout_time = -1;
                while(gGPS_Point_Cont.uiGPS_Point_Cout_time == -1){
                    usleep(100000);
                }
            }else if(1<=gGPS_Point_Cont.uiGPS_Point_Cout_time&&gGPS_Point_Cont.uiGPS_Point_Cout_time<=65535){
                starttime = 0, endtime = 0;
                GETCLOCKSECOND(starttime);
                uiCur_Point_dis = GPS_Point_Judge();
                #if 1
                if(uiCur_Point_dis >= 0){
                    if(gGPS_Point_Cont.uiGPS_Point_cout == 1){
                        if(iPoint_mark != uiCur_Point_dis){
                            GPS_Point_Info_to_APP( uiCur_Point_dis);
                        }
                        iPoint_mark = uiCur_Point_dis;
                    }else{
                        if(bPoint_mark == false){
                            iPoint_mark = uiCur_Point_dis;
                            bPoint_mark = true;
                            uiSum_count--;
                        }
                        else{
                            uiSum_count--;
                            if(iPoint_mark == uiCur_Point_dis){
                                bPoint_remark   = true;
                            }
                            else{
                                bPoint_remark   = false;
                                uiSum_count     = gGPS_Point_Cont.uiGPS_Point_cout;
                                iSend_mark      = false;
                            }
                            if((uiSum_count == 0)&&(bPoint_remark == true)){
                                if(iSend_mark == false){
                                    GPS_Point_Info_to_APP( uiCur_Point_dis);
                                    iSend_mark  = true;
                                }
                                uiSum_count = gGPS_Point_Cont.uiGPS_Point_cout;
                                bPoint_remark = false;
                            }else{

                            }
                        }
                    }
                }else{
                    uiSum_count         = gGPS_Point_Cont.uiGPS_Point_cout;
                    iPoint_mark         = -1;
                    iSend_mark          = false;
                    bPoint_mark         = false;
                    bPoint_remark       = false;
                }
                #endif
                while(gGPS_Point_Cont.uiGPS_Point_Cout_time >= 1){
                    usleep(100000);
                    GETCLOCKSECOND(endtime);
                    if(gGPS_Point_Cont.bGPS_Point_On_OFF == false){
                        uiSum_count         = gGPS_Point_Cont.uiGPS_Point_cout;
                        iPoint_mark         = -1;
                        iSend_mark          = false;
                        bPoint_mark         = false;
                        bPoint_remark       = false;
                        GPS_PRINTF("gGPS_Point_Cont.bGPS_Point_On_OFF = %d\n",gGPS_Point_Cont.bGPS_Point_On_OFF);
                        break;
                    }else{
                        if((endtime - starttime) >= gGPS_Point_Cont.uiGPS_Point_Cout_time){
                            break;
                        }
                    }
                }
            }
        }
        usleep(100000);
    }
    

}
/*****************************************************************************
** 函数名称 : GPS_Deviated_Route
** 功能描述 : 处理路线偏移
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static  int GPS_Point_Judge ()
{   
    double dStart_x,dStart_y, dCur_P_x,dCur_P_y;
    unsigned int uiRet_Cur_distance = 0;
    //允许超过范围的最大距离
    bool         bAlarm_ON_OFF      = false;   
    unsigned int uiMax_distance     = gGPS_Point_Cont.uiGPS_Point_distance;
    int          iSum_piont         = gvecP_Point.size();
    int          iCharacter_LON     = 1;
    int          iCharacter_LAT     = 1;
    //当前点转换为坐标
    if((GPS_gprmc.GPS_Longitude_symbol[0] == 'W') ||(GPS_gprmc.GPS_Longitude_symbol[0] == 'w')){
        iCharacter_LON          = -1;
    }
    else{
        iCharacter_LON          = 1;
    }
    dCur_P_x            = atof(GPS_gprmc.GPS_APP_Longitude)*iCharacter_LON; 
    if((GPS_gprmc.GPS_Latitude_sign[0] == 'S') ||(GPS_gprmc.GPS_Latitude_sign[0] == 's')){
        iCharacter_LAT          = -1;
    }
    else{
        iCharacter_LAT          = 1;
    }
    dCur_P_y                    = atof(GPS_gprmc.GPS_APP_Latitude)*iCharacter_LAT; 
    if((dCur_P_y == 0)||(dCur_P_x == 0)){
        return GPS_Point_E;
    }

    dCur_P_x                    = dCur_P_x*cos((dCur_P_y/1000000.0)*0.0174533)*0.111322;
    dCur_P_y                    = dCur_P_y*0.111133;
    //线路点
    //判断点的垂直线是否在线段上4294967296 赤道长度：40,076千米 1度111.322KM 地球的子午线总长度大约40008km。平均111.133KM
    struct GPS_Point_data GPS_Point;                     //路线点
    for(int i = 0;i < gvecP_Point.size();i++){
        
        GPS_Point               = gvecP_Point[i];
        
        if((GPS_Point.uiGPS_Point_lo_sign== 'W')||(GPS_Point.uiGPS_Point_lo_sign == 'w'))
            iCharacter_LON          = -1;
        else
            iCharacter_LON          = 1;
        dStart_x                    = iCharacter_LON*GPS_Point.uiGPS_Point_Longitude*cos((GPS_Point.uiGPS_Point_Latitude/1000000.0)*0.0174533)*0.111322;

        if((GPS_Point.uiGPS_Point_la_sign == 'S')||(GPS_Point.uiGPS_Point_la_sign == 's'))
            iCharacter_LAT          = -1;
        else
            iCharacter_LAT          = 1;
        dStart_y                    = iCharacter_LAT*GPS_Point.uiGPS_Point_Latitude*0.111133;
        

        uiRet_Cur_distance          = GPS_Point_to_Point_dis(dStart_x,dStart_y, dCur_P_x,dCur_P_y); 

        //GPS_PRINTF("point %8d  %8d %d\n",uiRet_Cur_distance,uiMax_distance,i);
        if(uiRet_Cur_distance < uiMax_distance){
            return i;
            //break;
        }else{
            
            //break;
        }
    }
    return GPS_Point_E;
}



/*****************************************************************************
** 函数名称 : GPS_Point_to_Point_dis
** 功能描述 : 求点到直线的垂直线是否在线段上
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
unsigned int GPS_Point_to_Point_dis(double dPoint_X,double dPoint_Y,double dCur_Point_X,double dCur_Point_Y) 
{

    bool     ret_len = false;
    //判断点的垂直线是否在线段上
    //计算点到线段起点的距离 
    double  dOffsetX                    = dCur_Point_X - dPoint_X; 
    double  dOffsetY                    = dCur_Point_Y - dPoint_Y; 
    double  dbSquare_Of_Distance        = pow(fabs(dOffsetX),2)+pow(fabs(dOffsetY),2); //
    double  dbPoint_To_ptStartDistance  = sqrt(dbSquare_Of_Distance); 
    return (int)(dbPoint_To_ptStartDistance+0.5);

}
/*****************************************************************************
** 函数名称 : GPS_Point_Info_to_APP
** 功能描述 : 发送数据到app
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
static int GPS_Point_Info_to_APP( int  uiDistance){
    char sDistance[12];
    
    sprintf(sDistance,"%d",uiDistance);
    //发送经度，纬度，高度，数度，信号强度，纬度符号，经度符号，时间
    g_AppMsg->Send(APP_MSG,MSG_GPS_APP_IN_POINT,string(sDistance)+MSG_SEPARATOR+GPS_gprmc.GPS_APP_Latitude+MSG_SEPARATOR+GPS_gprmc.GPS_APP_Longitude+
        MSG_SEPARATOR+GPS_gprmc.GPS_Latitude_sign+MSG_SEPARATOR+GPS_gprmc.GPS_Longitude_symbol);

    return GPS_OK;
}
/*****************************************************************************
** 函数名称 : GPS_Other_place_discharge
** 功能描述 : 异地卸载
** 输   入  :  :
**          : 
**            
** 输　 出  : 
**            
** 全局变量 :
** 调用模块 : 
**
** 作　者: 侯朝能
** 日　期: 
**----------------------------------------------------------------------------
** 修改人:
** 日　期:
**----------------------------------------------------------------------------
*****************************************************************************/
int     GPS_Other_place_discharge(vector<GprsUnLoadPoint> vec_GprsUnLoadPoint){
    struct GprsUnLoadPoint gps_UnloadPoint;
    double dStart_x,dStart_y, dCur_P_x,dCur_P_y;
    unsigned int uiRet_Cur_distance = 0;
    //允许超过卸载范围的最大距离
    int          iSum_piont         = vec_GprsUnLoadPoint.size();
    if((vec_GprsUnLoadPoint.size() <= 0 )||((vec_GprsUnLoadPoint.size() > 10 ))){
        GPS_PRINTF("Point Init size = %d error!\n",vec_GprsUnLoadPoint.size());
        return GPS_Point_E_INIT;
    }

    int          iCharacter_LON     = 1;
    int          iCharacter_LAT     = 1;
    //当前点转换为坐标
    if((GPS_gprmc.GPS_Longitude_symbol[0] == 'W') ||(GPS_gprmc.GPS_Longitude_symbol[0] == 'w')){
        iCharacter_LON          = -1;
    }
    else{
        iCharacter_LON          = 1;
    }
    dCur_P_x            = atof(GPS_gprmc.GPS_APP_Longitude)*iCharacter_LON; 
    if((GPS_gprmc.GPS_Latitude_sign[0] == 'S') ||(GPS_gprmc.GPS_Latitude_sign[0] == 's')){
        iCharacter_LAT          = -1;
    }
    else{
        iCharacter_LAT          = 1;
    }
    dCur_P_y                    = atof(GPS_gprmc.GPS_APP_Latitude)*iCharacter_LAT; 
    if((dCur_P_y == 0)||(dCur_P_x == 0)){
        return GPS_Point_E;
    }

    dCur_P_x                    = dCur_P_x*cos((dCur_P_y/1000000.0)*0.0174533)*0.111322;
    dCur_P_y                    = dCur_P_y*0.111133;
    //判断点的垂直线是否在线段上4294967296 赤道长度：40,076千米 1度111.322KM 地球的子午线总长度大约40008km。平均111.133KM
    for(int i = 0;i < iSum_piont;i++){
        
        gps_UnloadPoint             = vec_GprsUnLoadPoint[i];
        unsigned int uiMax_distance = vec_GprsUnLoadPoint[i].distance;

        if((gps_UnloadPoint.mappoint.longitude_sign== 'W')||(gps_UnloadPoint.mappoint.longitude_sign == 'w'))
            iCharacter_LON          = -1;
        else
            iCharacter_LON          = 1;

        if((gps_UnloadPoint.mappoint.longitude<0)||(gps_UnloadPoint.mappoint.longitude>180000000)){
            GPS_PRINTF("Unload Init longitude= %d error!\n",gps_UnloadPoint.mappoint.longitude);
            return GPS_Point_E_INIT;    
        }
        
        dStart_x                    = iCharacter_LON*gps_UnloadPoint.mappoint.longitude*cos((gps_UnloadPoint.mappoint.latitude/1000000.0)*0.0174533)*0.111322;

        if((gps_UnloadPoint.mappoint.latitude_sign== 'S')||(gps_UnloadPoint.mappoint.latitude_sign == 's'))
            iCharacter_LAT          = -1;
        else
            iCharacter_LAT          = 1;


        if((gps_UnloadPoint.mappoint.latitude<0)||(gps_UnloadPoint.mappoint.latitude>90000000)){
            GPS_PRINTF("Unload Init Latitude= %d error!\n",gps_UnloadPoint.mappoint.latitude);
            return GPS_Point_E_INIT;    
        }

        dStart_y                    = iCharacter_LAT*gps_UnloadPoint.mappoint.latitude*0.111133;
        

        uiRet_Cur_distance          = GPS_Point_to_Point_dis(dStart_x,dStart_y, dCur_P_x,dCur_P_y); 

        //GPS_PRINTF("unload point %8d  %8d %d\n",uiRet_Cur_distance,uiMax_distance,i);
        if(uiRet_Cur_distance < uiMax_distance){
            return i+1;
            //break;
        }else{
            
            //break;
        }
    }
    return 0;

    
}

int GPS_rail_init(vector<GprsRail> vec_GprsRail){
	///@brief
	///初始化电子围栏数据
	sGprsRail.index = 0;
	sGprsRail.func  = 0;
	sGprsRail.inorout =0;
    for(int i = 0;i < 1024;i++)
    {
        bRail_Send[i] = false;
    }
    gvec_GprsRail.clear();
    gvec_GprsRail                                  = vec_GprsRail;
    int point_sum = gvec_GprsRail.size();
    if(point_sum > 1024)
    {
        GPS_PRINTF("Gps_Rail_data point_sum = %d error!\n",point_sum);
        return GPS_RAIL_E_INIT;    
    }
    struct GprsRail Gps_Rail_data;
    for(int i = 0;i < point_sum;i++){
        Gps_Rail_data = gvec_GprsRail[i];
        //GPS_PRINTF("point[%d] =  %10d %10d\n",Gps_Rail_data.index,Gps_Rail_data.top.longitude,Gps_Rail_data.top.latitude);
        //GPS_PRINTF("point[%d] =  %10d %10d\n",Gps_Rail_data.index,Gps_Rail_data.bottom.longitude,Gps_Rail_data.bottom.latitude);
        //GPS_PRINTF("point[%d] =  %10d \n",Gps_Rail_data.index,Gps_Rail_data.inorout);
        if((Gps_Rail_data.top.latitude<0)||(Gps_Rail_data.top.latitude > 90000000)){
            GPS_PRINTF("Gps_Rail_data.top.altitude = %d error!\n",Gps_Rail_data.top.latitude);
            return GPS_RAIL_E_INIT;    
        }
        if((Gps_Rail_data.top.longitude < 0)||(Gps_Rail_data.top.longitude > 180000000)){
            GPS_PRINTF("Gps_Rail_data.top.longitude = %d error!\n",Gps_Rail_data.top.longitude);
            return GPS_RAIL_E_INIT;    
        }
        if((Gps_Rail_data.bottom.latitude<0)||(Gps_Rail_data.bottom.altitude>90000000)){
            GPS_PRINTF("Gps_Rail_data.bottom.altitude = %d error!\n",Gps_Rail_data.bottom.latitude);
            return GPS_RAIL_E_INIT;    
        }
        if((Gps_Rail_data.bottom.longitude<0)||(Gps_Rail_data.bottom.longitude>180000000)){
            GPS_PRINTF("Gps_Rail_data.bottom.longitude = %d error!\n",Gps_Rail_data.bottom.longitude);
            return GPS_RAIL_E_INIT;    
        }
    }
    if(bRail_init_flag == false)
    {
        pthread_t     pid_GPS_rail;                           //数据处理线程
        pthread_create(&pid_GPS_rail,NULL,GPS_rail,0);
        bRail_init_flag == true;
    }
    
}

static void * GPS_rail(void *arg){
    bool    bRail_on_off = false;
    
    while(1){
        if(gvec_GprsRail.size() <= 0 )
        {
            bRail_on_off = false;
        }
        else
        {
            bRail_on_off = true;
        }
        
        if(bRail_on_off == true)
        {
            GPS_rail_Judge();
        }
        sleep(1);

    }
}

static  int     GPS_rail_Judge()
{
    double      dCur_P_x = 0,dCur_P_y = 0;
    int         iCharacter_LON     = 1;
    int         iCharacter_LAT     = 1;
    if((GPS_gprmc.GPS_Longitude_symbol[0] == 'W') ||(GPS_gprmc.GPS_Longitude_symbol[0] == 'w'))
    {
        iCharacter_LON          = -1;
    }
    else
    {
        iCharacter_LON          = 1;
    }
    dCur_P_x            = atof(GPS_gprmc.GPS_APP_Longitude)*iCharacter_LON; 
    
    if((GPS_gprmc.GPS_Latitude_sign[0] == 'S') ||(GPS_gprmc.GPS_Latitude_sign[0] == 's'))
    {
        iCharacter_LAT          = -1;
    }
    else
    {
        iCharacter_LAT          = 1;
    }
    dCur_P_y                    = atof(GPS_gprmc.GPS_APP_Latitude)*iCharacter_LAT; 

    if((dCur_P_y == 0)||(dCur_P_x == 0)){
		sGprsRail.index 	= 0;
		sGprsRail.func  	= 0;
		sGprsRail.inorout	= 0;
		return -1;
    }
		///@brief
	    ///开始电子围栏的判断
    GPS_rail_Point_convert(dCur_P_x,dCur_P_y);    
}

static  int     GPS_rail_Point_convert(double cur_x,double cur_y)
{
    int         iSum_piont          = gvec_GprsRail.size();
    double      dTop_x              = 0,
                dTop_y              = 0, 
                dBottom_x           = 0,
                dBottom_y           = 0;  
    unsigned     char   range=0x00;
    struct GprsRail Rail_data;
    for(int i = 0;i < iSum_piont;i++)
    {
        Rail_data       = gvec_GprsRail[i];
        if((Rail_data.top.longitude_sign== 'W')||(Rail_data.top.longitude_sign == 'w'))
        {
            dTop_x      = (-1)*Rail_data.top.longitude;
        }
        else
        {
            dTop_x      = Rail_data.top.longitude;
        }
        
        if((Rail_data.top.latitude_sign== 'S')||(Rail_data.top.latitude_sign == 's'))
        {
            dTop_y      = (-1)*Rail_data.top.latitude;
        }
        else
        {
            dTop_y      = Rail_data.top.latitude;
        }
        

        if((Rail_data.bottom.longitude_sign== 'W')||(Rail_data.bottom.longitude_sign == 'w'))
        {
            dBottom_x      = (-1)*Rail_data.bottom.longitude;
        }
        else
        {
            dBottom_x      = Rail_data.bottom.longitude;
        }
        
        if((Rail_data.bottom.latitude_sign== 'S')||(Rail_data.bottom.latitude_sign == 's'))
        {
            dBottom_y      = (-1)*Rail_data.bottom.latitude;
        }
        else
        {
            dBottom_y      = Rail_data.bottom.latitude;
        }
        ////////////////////////////////////////////////////////////////////////
        //GPS_PRINTF("%10.0f %10.0f\n",dTop_x,dTop_y);
        //GPS_PRINTF("%10.0f %10.0f\n",cur_x,cur_y);
        //GPS_PRINTF("%10.0f %10.0f\n\n",dBottom_x,dBottom_y);
        if(((cur_x < dTop_x) ||(cur_x > dBottom_x))||
           ((cur_y > dTop_y) ||(cur_y < dBottom_y)))
        {
            if(bRail_Send[i] != false)
            {
                GPS_PRINTF("index = %d inorout = %d func=%d bRail_Send = %d outrail\n",
                            Rail_data.index,Rail_data.inorout,Rail_data.func,bRail_Send[i]);
                GPS_rail__Alarm_to_APP( Rail_data.index,Rail_data.inorout,Rail_data.func,0x01);
                bRail_Send[i] = false;
            }
			sGprsRail.index 	= 0;
			sGprsRail.func  	= 0;
			sGprsRail.inorout	= 0;
		}
		else
		{
            if(bRail_Send[i] != true)
            {
                GPS_PRINTF("index = %d inorout = %d func=%d bRail_Send = %d inrail\n",
                            Rail_data.index,Rail_data.inorout,Rail_data.func,bRail_Send[i]);
                GPS_rail__Alarm_to_APP( Rail_data.index,Rail_data.inorout,Rail_data.func,0x02);
                bRail_Send[i] = true;
            }
			sGprsRail.index 	= Rail_data.index;
			sGprsRail.func  	= Rail_data.func;
			sGprsRail.inorout	= Rail_data.inorout;
			break;
        }
    }
}

static  int     GPS_rail__Alarm_to_APP( unsigned int  index,unsigned char inorout, unsigned short func,unsigned char armType)
{
    char cIndex[12] , cInorout[12];
    sprintf(cIndex,"%d",index);
    string strInOrOut, strFunc;
    if(inorout == 0x01)         //内 -> 外
        strInOrOut = "58";
    else if(inorout == 0x02)    //外 -> 内
        strInOrOut = "57";  
    else if(0x03 == inorout)
    {
        if(0x01 == armType )
        {
            strInOrOut = "58";
        }
        if(0x02 == armType)
        {
            strInOrOut = "57";
        }

    } else {
        return -1;
    }

    if(func == 0x01)            //防爆禁区
        strFunc = "0";
    else if(func == 0x02)       //区域内卸载称重
        strFunc = "1";
    else if(func == 0x04)
        strFunc = "2";          //区域内装载称重
    else 
        return -1;
		///@brief
		///发送索引，进入或者出来报警类型，纬度，经度，纬度符号，经度符号，时间
    g_AppMsg->Send(APP_MSG,MSG_GPS_RAIL,string(cIndex)+MSG_SEPARATOR+strInOrOut+MSG_SEPARATOR+strFunc);
    g_AppMsg->Send(GUI_MSG,MSG_GPS_RAIL,string(cIndex)+MSG_SEPARATOR+strInOrOut+MSG_SEPARATOR+strFunc);
    
    return GPS_OK;
    
}


	///@brief
	///判断是否异地装卸载
	///@parm 0x0001 - 防爆判断 0x0002 卸载判断 0x0004 装载判断

int GPS_rail_Load_or_unload(unsigned short  func)
{
	switch(func)
	{
		case 0x0001:		///防爆禁区
			break;
		case 0x0002:		///卸载称重判断
			if(0x0002 == sGprsRail.func)
			{
				break;
			}
			else
			{
				GPS_Unload_Alarm_to_APP();
			}
			break;
		case 0x0004:		///装载称重
			if(0x0004 == sGprsRail.func)
			{
				break;
			}
			else
			{
				GPS_Load_Alarm_to_APP();
			}
			break;
		default:
			break;
	}
}

	///@brief
	///异地卸载报警
static  int     GPS_Unload_Alarm_to_APP()
{
    GPS_PRINTF("Other Unload sGprsRail.func=%10d sGprsRail.index = %d\n",sGprsRail.func,sGprsRail.index);

	GPSAlarmChange(A_POS_DOWNLOAD,true);
    if(AlarmFlagGPS&A_POS_DOWNLOAD) {       //add by unreal 2010-11-30 A_POS_DOWNLOAD只报一次
        AlarmFlagGPS &= ~A_POS_DOWNLOAD;
    }    
	
}
	///@brief
	///异地装载报警
static  int     GPS_Load_Alarm_to_APP()
{
    DEBUGPRINT("Other Load sGprsRail.func=%10d sGprsRail.index = %d\n",sGprsRail.func,sGprsRail.index);
    GPSAlarmChange(A_POS_LOAD,true);
    if(AlarmFlagGPS & A_POS_LOAD) {         //add by unreal 2010-11-30 A_POS_LOAD只报一次
        AlarmFlagGPS &= ~A_POS_LOAD;
    }    

}


///@brief
///报警状态改变
int     GPSAlarmChange(unsigned int FLAG,bool AlarmIng)
{
    pthread_mutex_lock(&mut);
    bool    willSend = false;
    
    switch(FLAG)
    {
        case A_POS_SIGNAL:                  //GPS无信号超时报警取消其他所有报警
        {
            if(true == AlarmIng)
            {
                if((AlarmFlagGPS&A_POS_SIGNAL) != A_POS_SIGNAL)
                {
                    AlarmFlagGPS = (AlarmFlagGPS|FLAG)&(~A_POS_DOWNLOAD)&(~A_POS_LINE)&(~A_POS_SPEED)&(~A_POS_STOPTIME)&(~A_POS_LOAD);
                    willSend = true;
                }
                
            }
            if(false == AlarmIng)
            {
                if((AlarmFlagGPS&A_POS_SIGNAL) == A_POS_SIGNAL)
                {
                    AlarmFlagGPS = AlarmFlagGPS&~FLAG;
                    willSend = true;
                }
            }

            break;
        }
        case A_POS_SPEED:                   //速度超限报警 解除停车超时
        {
            if((AlarmFlagGPS&A_POS_SIGNAL) == A_POS_SIGNAL)
            {   
                break;
            }
            if(true == AlarmIng)
            {
                if((AlarmFlagGPS&A_POS_SPEED) != A_POS_SPEED)
                {
                    AlarmFlagGPS = (AlarmFlagGPS|FLAG)&(~A_POS_STOPTIME);
                    willSend = true;

                }
            }
            if(false == AlarmIng)
            {
                if((AlarmFlagGPS&A_POS_SPEED) == A_POS_SPEED)
                {
                    AlarmFlagGPS = AlarmFlagGPS&~FLAG;
                     willSend = true;
                }
            }

            break;
        }
        case A_POS_STOPTIME:                //汽车停止超时 解除速度超速报警
        {
            if((AlarmFlagGPS&A_POS_SIGNAL) == A_POS_SIGNAL)
            {   
                break;
            }

            if(true == AlarmIng)
            {
                if((AlarmFlagGPS&A_POS_STOPTIME) != A_POS_STOPTIME)
                {
                    AlarmFlagGPS = (AlarmFlagGPS|FLAG)&(~A_POS_SPEED);
                    willSend = true;
                }
            }
            if(false == AlarmIng)
            {
                if((AlarmFlagGPS&A_POS_STOPTIME) == A_POS_STOPTIME)
                {
                    AlarmFlagGPS = AlarmFlagGPS&~FLAG;
                    willSend = true;
                }
            }

            break;
        }
        case A_POS_LINE:                    //偏离路线报警
        {
            if((AlarmFlagGPS&A_POS_SIGNAL) == A_POS_SIGNAL)
            {   
                break;
            }
            if(true == AlarmIng)
            {
                if((AlarmFlagGPS&A_POS_LINE) != A_POS_LINE)
                {
                    AlarmFlagGPS = (AlarmFlagGPS|FLAG);
                    willSend = true;
                }
            }
            if(false == AlarmIng)
            {
                if((AlarmFlagGPS&A_POS_LINE) == A_POS_LINE)
                {
                    AlarmFlagGPS = AlarmFlagGPS&~FLAG;
                    willSend = true;
                }
            }

            break;
        }
        case A_POS_DOWNLOAD:                //异地卸载报警
        {
            if((AlarmFlagGPS&A_POS_SIGNAL) == A_POS_SIGNAL)
            {   
                break;
            }

            if(true == AlarmIng)
            {
                if((AlarmFlagGPS&A_POS_DOWNLOAD) != A_POS_DOWNLOAD)
                {
                    AlarmFlagGPS = (AlarmFlagGPS|FLAG);
                    willSend = true;
                }
            }
            if(false == AlarmIng)
            {
                if((AlarmFlagGPS&A_POS_DOWNLOAD) == A_POS_DOWNLOAD)
                {
                    AlarmFlagGPS = AlarmFlagGPS&~FLAG;
                    willSend = true;
                }
            }
            break;
        }
        case A_POS_LOAD:                    //异地装载报警
        {
            if((AlarmFlagGPS&A_POS_SIGNAL) == A_POS_SIGNAL)
            {   
                break;
            }

            if(true == AlarmIng)
            {
                if((AlarmFlagGPS&A_POS_LOAD) != A_POS_LOAD)
                {
                    AlarmFlagGPS = (AlarmFlagGPS|FLAG);
                    willSend = true;
                }
            }
            if(false == AlarmIng)
            {
                if((AlarmFlagGPS&A_POS_LOAD) == A_POS_LOAD)
                {
                    AlarmFlagGPS = AlarmFlagGPS&~FLAG;
                    willSend = true;
                }
            }

            break;
        }

    }
    char alarm[16] ="00000000";
    sprintf(alarm,"%08x",AlarmFlagGPS);
     printf("willSend = %d  AlarmFlagGPS = %08x alarm = %s\n",willSend,AlarmFlagGPS,alarm);
    if(true == willSend)
    {
        g_AppMsg->Send(APP_MSG,MSG_GPS_APP_ALARM,string(alarm));//发送报警
    }
    pthread_mutex_unlock(&mut);
}


