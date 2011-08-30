//////////////////////////////////////////////////////////////////////

#ifndef __GPS_H
#define __GPS_H
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "../main/dt_gprshead.h"
using namespace std;
#define DEFAULT_GPSD_PORT    "2947"    /* IANA assignment */
#define DEFAULT_RTCM_PORT    "2101"    /* IANA assignment */
#define GETCLOCKSECOND(x) {                \
        x= time(NULL);                \
    }
/*监控中心向终端发送图片信息*/
struct GPS_INIT_DATA{
        unsigned int uiPicture_UP_Longitude;    //图片左上角经度
        unsigned int uiPicture_UP_Latitude;     //图片左上角纬度
        unsigned int uiPicture_DOWN_Longitude;  //图片右下角经度
        unsigned int uiPicture_DOWN_Latitude;   //图片右下角纬度

        unsigned short usPicture_UP_X_Pixel;    //图片左上角x值
        unsigned short usPicture_UP_Y_Pixel ;   //图片左上角Y方向
        unsigned short usPicture_DOWN_X_Pixel;  //图片右下角X方向
        unsigned short usPicture_DOWN_Y_Pixel;  //图片右下角Y方向
        unsigned short usPicture_X_Pixel;       //图片X轴像素大小
        unsigned short usPicture_Y_Pixel  ;     //图片Y轴像素大小

        unsigned short usGPS_TTI;               //控制向主控程序发送GPS信息间隔
        unsigned short usGPS_GUI_TTI;           //控制向GUI发送GPS信息间隔

        char cPicture_UP_Longitude_Symbols;     //图片左上角经度符号
        char cPicture_UP_Latitude_Symbols;      //图片左上角纬度符号
        char cPicture_DOWN_Longitude_Symbols;   //图片右下角经度符号
        char cPicture_DOWN_Latitude_Symbols;    //图片右下角纬度呼号
};      
/*监控中心向终端发送轨迹信息*/
struct GPS_Route_Point_data{
        unsigned char   uiGPS_Route_la_sign;    //纬度符号
        unsigned int    uiGPS_Route_Latitude;   //路线坐标点纬度值
        unsigned char   uiGPS_Route_lo_sign;    //经度符号
        unsigned int    uiGPS_Route_Longitude;  //路线坐标点经度值
        unsigned int    uiGPS_Route_altitude;   //高度
        unsigned int    uiGPS_Route_X;          //地图X坐标
        unsigned int    uiGPS_Route_Y;          //纬度Y坐标
};
/*监控中心向终端发送轨迹控制信息*/
struct GPS_Route_data{
        unsigned int    uiGPS_Offset_distance;  //当前位置距离路线的最短偏移距离
        int             uiGPS_Offset_cout;      //当偏移多少次后产生报警
        int             uiGPS_Offset_Cout_time; //多时间计算一次报警状态
        bool            bOn_OFF;                //设置开关
};
/*监控中心向终端发送超速信息*/
struct GPS_Speed_Point_data{
        GPS_Route_Point_data    start;          //该段起点
        GPS_Route_Point_data    end;            //该段止点
        unsigned int    maxspeed;               //该段最大速度
};
/*监控中心向终端发送超速控制信息*/
struct GPS_Speed_Cont_data{
        int     uiGPS_Speed_cout;               //当超速多少次后产生报警
        int     uiGPS_Speed_Cout_time;          //多时间计算一次报警状态
        int     uiGPS_ALL_Max_Speed;            //全路段最大速度
        bool    bGPS_Speed_On_OFF;              //设置开关
};
/*监控中心向终端发送到点信息*/
struct GPS_Point_data
{
        unsigned char   uiGPS_Point_la_sign;    //纬度符号,'N'或'S'
        unsigned int    uiGPS_Point_Latitude;   //纬度
        unsigned char   uiGPS_Point_lo_sign;    //经度符号,'E'或'W'
        unsigned int    uiGPS_Point_Longitude;  //经度
        unsigned int    uiGPS_Point_altitude;   //高度
        unsigned int    uiGPS_Point_X;          //地图X坐标
        unsigned int    uiGPS_Point_Y;          //纬度Y坐标
};
/*监控中心向终端发送到点控制信息*/
struct GPS_Point_Contrl {
        unsigned int    uiGPS_Point_distance;   //当前位置距离路线的最短偏移距离
        int             uiGPS_Point_cout;       //当偏移多少次后产生报警
        int             uiGPS_Point_Cout_time;  //多时间计算一次报警状态
        bool            bGPS_Point_On_OFF;      //设置开关
};
/*监控中心向终端发送异地卸载信息*/
struct GPS_other_places
{
        unsigned char   uiGPS_Point_la_sign;    //纬度符号,'N'或'S'
        unsigned int    uiGPS_Point_Latitude;   //纬度
        unsigned char   uiGPS_Point_lo_sign;    //经度符号,'E'或'W'
        unsigned int    uiGPS_Point_Longitude;  //经度
        unsigned int    uiGPS_Point_altitude;   //高度
        unsigned int    uiGPS_Point_X;          //地图X坐标
        unsigned int    uiGPS_Point_Y;          //纬度Y坐标
};


int             GPS_INIT(void);
int             GPS_Control( struct GPS_INIT_DATA GPS_INIT_data1);
int             GPS_CUR_LO_LA_DATA( GpsMapPoint& la_lo);
int             gpsclose();
static void *   getgps(void *arg);
static int      GPS_Data_decomposition(char *cRead_buf);
static void     GPS_Init_gprmc(void);
static int      GPS__INIT_Client(int *sock);
static int      netlib_connectsock(const char *host, const char *service, const char *protocol);
static int      get_time(char time_years[12],char time_hhmmss[12]);
static int      getN_E_S_W(char north_south[12],char est_wth[12]);
static int      GPS_TO_GUI(char north_south[12],char est_wth[12],char weidu_log[12],char jingdu_log[12]);



static double   GPS_F_jingdu(char north_south[12]);
static double   GPS_F_weidu(char north_south[12]);
static int      GPS_Analysis_Data(int *sock);
static int      GPS_Send_Data_GUI( void);
int			    GPS_Send_Data_APP( void);
static void *   GPS_DATA_to_GUI_treatment(void *arg);
static void *   GPS_DATA_to_APP_treatment(void *arg);
//越界报警
int             GPS_Deviated_Route_init(vector<GPS_Route_Point_data> vecPoint_data, struct GPS_Route_data GPS_Offset_data);
int             GPS_Route_Contrl_init(vector<GPS_Route_Point_data> vecPoint_data, struct GPS_Route_data GPS_Offset_data);
static void *   GPS_Route_Contrl(void *arg);
static unsigned int     GPS_Route_Point_range(double dStart_X,double dStart_Y,double dEnd_X,double dEnd_Y,double dCur_P_X,double dCur_P_Y) ;
static unsigned int     GPS_Route_range ();
static int      GPS_Rotue_Info_to_APP( unsigned int  uiDistance);
//超速报警
int             GPS_Speed_Limit_init(vector<GPS_Speed_Point_data> vecS_Point,struct GPS_Speed_Cont_data GPS_Speed_Cont);
int             GPS_Speed_Contrl_init(vector<GPS_Speed_Point_data> vecS_Point,struct GPS_Speed_Cont_data GPS_Speed_Cont);
static void *   GPS_Spped_Limit(void *arg);
static  int     GPS_Speed_range ();
static unsigned int    GPS_Line_Speed_range(double dStart_X,double dStart_Y,double dEnd_X,double dEnd_Y,double dCur_P_X,double dCur_P_Y) ;
static int      GPS_Speed_Alarm_to_APP( int  speed);
//到点报警
int             GPS_Point_init(vector<GPS_Point_data> vec_Point,struct GPS_Point_Contrl GPS_Point_Cont);
int             GPS_Point_Contrl_init(vector<GPS_Point_data> vec_Point,struct GPS_Point_Contrl GPS_Point_Cont);
static void *   GPS_Point_Limit(void *arg);
static  int     GPS_Point_Judge();
static unsigned int    GPS_Point_to_Point_dis(double dPoint_X,double dPoint_Y,double dCur_Point_X,double dCur_Point_Y) ;
static int      GPS_Point_Info_to_APP( int  uiDistance);

//判断停止时长
int             GPS_Stop_Max_time(unsigned int time);
static void *   GPS_Stop_time(void *arg);
//判断无信号时长
int             GPS_No_signal_Max_time(unsigned int time);
static void *   GPS_No_signal_time(void *arg);
//异地卸载
int             GPS_Other_place_discharge(vector<GprsUnLoadPoint> vec_GprsUnLoadPoint);
///@brief
///电子围栏
int             GPS_rail_init(vector<GprsRail> vec_GprsRail);
static void *   GPS_rail(void *arg);
static  int     GPS_rail_Judge();

static  int     GPS_rail_Point_convert(double cur_x,double cur_y);
static  int     GPS_rail__Alarm_to_APP( unsigned int  index,unsigned char inorout, unsigned short func,unsigned char armType);
///@brief
///电子围栏里卸载与转载判断
///@parm 0x0001 - 防爆判断 0x0002 卸载判断 0x0004 装载判断
int             GPS_rail_Load_or_unload(unsigned short  func);
///@brief
///发送异地卸载报警
static  int     GPS_Unload_Alarm_to_APP();

///@brief
///发送异地装载报警
static  int     GPS_Load_Alarm_to_APP();

///@brief
///报警状态改变
int     GPSAlarmChange(unsigned int FLAG,bool AlarmIng);


#endif // !defined(__GPS_H)
