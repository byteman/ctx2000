#include "comdata.h"
#include <math.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <stdarg.h>

TDispParam::TDispParam(double span):
    m_span(span)
{
    prev = 1234567;
}
bool TDispParam::need_update(double value)
{
    if(fabs(value-prev) < m_span) return false;
    prev = value;
    return true;
}
TDispParam gDispFilter[8];
TBD g_bd[13];
TID g_valid_tj_list;
TID g_conflict_tj_list;
std::string MainMachineID,CurID,TCTypeName,CurSerial;
int    RightNo,AddNo,MaxTCNo,g_local_id;
int    TCArmLen,g_tc_rate;
int    gMainMenuIndex=1;
std::string StrTCArmLen, StrTCRate;
double wksp[4][2];
double czwzb[2][2];
double YNAngle,VStopDis,VWarnDis,BrakeDis,DangerDis,WarnDis,AddAngle;
int    FDTime,BrakeTime;
Twb    wba[11];
TDividePoint DividePoint[21];
int TcNum,wbNum=10,DividePointNum,TCTotalNum,fd_time,brake_time;
unsigned char tcorder[21];
double Trolly_dist_min,Trolly_dist_max,V0;
int ad_angle, ad_up_angle, ad_car_dist, ad_weight,ad_angle_x,ad_angle_y,ad_wild_speed, ad_height;
TTjRecord g_TC[21];
double g_car_dist,g_dg_height,g_dg_weight,g_angle,g_wild_speed,g_up_angle,g_angle_x,g_angle_y;
double g_rated_weight=0;
double g_angle_A,g_angle_B,g_angle_C;
std::string g_diantai_com,g_ad_com,g_encoder_com,g_gprs_com,g_gps_com;
int encoder_addr;
//gprs===
std::string gprs_remote_ip;
std::string gprs_dtu_id;
int gprs_remote_port;
bool g_show_up_angle;
bool g_show_speed;
bool g_show_dg_height;
bool g_show_max_weight;
int  g_alarm_wild_speed=13;
bool g_show_lang_zh=true;
/**************************ʱת5k**************************/
double          tmp_wksp[4][2]; //64
double          tmp_czwzb[2][2]; //32
double          tmp_YNAngle, tmp_VStopDis,tmp_VWarnDis,tmp_BrakeDis,tmp_DangerDis; //48
double          tmp_WarnDis,tmp_AddAngle; //24
TTjRecord       tmp_TC[21]; //200*21=4000k
TDividePoint    tmp_DividePoint[21]; //16x21
Twb             tmp_wba[11];
int             tmp_wbNum,tmp_TcNum,tmp_DividePointNum;
/*****************************************************************/
int set_thread_title(const char* fmt,... )
{
    char title [16] ={0};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf (title, sizeof (title) , fmt, ap);
    va_end (ap);

   return prctl(PR_SET_NAME,title) ;

}
void calc_angle(double bd[2][2],double x,double y,double &AngleA,double &AngleB,double &AngleC)
{
    double tempp;
    double x1 = bd[0][0] - x;
    double y1 = bd[0][1] - y;

    double x2 = bd[1][0] - x;
    double y2 = bd[1][1] - y;
    //fprintf(stderr,"x1=%0.2f,x2=%0.2f,y1=%0.2f,y2=%0.2f\n",x1,x2,y1,y2);
    if(y1==0)
    {
         if(x1>0)AngleA=0;
         else    AngleA=180;
    }
    if (x1==0)
    {
         if(y1>0) AngleA=90;
         else     AngleA=270;
    }
    if (y2==0)
    {
         if (x2>0) AngleB=0;
         else      AngleB=180;
     }
     if (x2==0){
          if (y2>0) AngleB=90;
          else      AngleB=270;
     }

    if( (x1 > 0) && (y1 > 0) ){
        //a1
        AngleA = atan(y1/x1)*180/3.14;
    }
    if( (x1 < 0) && (y1 > 0) ){
        double z= atan(1);
        fprintf(stderr,"z=%0.2f\n",z);
        AngleA = 180 - z*180/3.14;
    }
    if( (x1 < 0) && (y1 < 0) ){
        AngleA = 180 + atan(fabs(y1)/fabs(x1))*180/3.14;
    }
    if( (x1 > 0) && (y1 < 0) ){
        AngleA = 360-atan(fabs(y1)/x1)*180/3.14;
    }
/*
    if( (x2 > 0) && (y2 > 0) ){
        //a1
        AngleB = atan(y2/x2)*180/3.14;
    }
    if( (x2 < 0) && (y2 > 0) ){
        AngleB = 180 - atan(y2/fabs(x2))*180/3.14;
    }
    if( (x2 < 0) && (y2 < 0) ){
        AngleB = 180 + atan(fabs(y2)/fabs(x2))*180/3.14;
    }
    if( (x2 > 0) && (y2 < 0) ){
        AngleB = 360-atan(fabs(y2)/x2)*180/3.14;
    }
    */

    if( (x2 > 0) && (y2 > 0) ){
        //a1
        AngleB = 90- atan(x2/y2)*180/3.14;
        tempp=90-AngleB;
    }
    if( (x2 < 0) && (y2 > 0) ){
        AngleB = 180 - atan(y2/fabs(x2))*180/3.14;
        tempp=180-AngleB;
    }
    if( (x2 < 0) && (y2 < 0) ){
        AngleB = 270 - atan(fabs(x2)/fabs(y2))*180/3.14;
        tempp=270-AngleB;
    }
    if( (x2 > 0) && (y2 < 0) ){
        AngleB = 360-atan(fabs(y2)/x2)*180/3.14;
        tempp=360-AngleB;
    }

    if(AngleB > AngleA){
        AngleC =AngleB-AngleA;
    }else{
        AngleC=360-fabs(AngleB-AngleA);
    }

}
