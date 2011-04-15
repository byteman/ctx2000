#include "comdata.h"
#include <math.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <stdarg.h>

TBD g_bd[6];
TID g_valid_tj_list;
TID g_conflict_tj_list;
std::string MainMachineID,CurID,TCTypeName,CurSerial;
int    RightNo,AddNo,MaxTCNo,g_local_id;
int    TCArmLen,TCBeilv;
std::string StrTCArmLen, StrTCBeilv;
double wksp[4][2];
double czwzb[2][2];
double YNAngle,VStopDis,VWarnDis,BrakeDis,DangerDis,WarnDis,AddAngle;
Twb    wba[11];
TDividePoint DividePoint[21];
int TcNum,wbNum,DividePointNum,TCTotalNum,fd_time,brake_time;
unsigned char tcorder[21];
double Trolly_dist_min,Trolly_dist_max,V0;
int ad_angle, ad_up_angle, ad_car_dist, ad_weight,ad_fengsu, ad_height;
TTjRecord g_TC[21];
double g_car_dist,g_dg_height,g_dg_weight,g_angle,g_speed,g_up_angle;
double g_angle_A,g_angle_B,g_angle_C;
std::string g_diantai_com,g_ad_com1,g_ad_com2,g_gprs_com;
int encoder_addr;

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
        AngleA = 180 + atan(abs(y1)/abs(x1))*180/3.14;
    }
    if( (x1 > 0) && (y1 < 0) ){
        AngleA = 360-atan(abs(y1)/x1)*180/3.14;
    }
/*
    if( (x2 > 0) && (y2 > 0) ){
        //a1
        AngleB = atan(y2/x2)*180/3.14;
    }
    if( (x2 < 0) && (y2 > 0) ){
        AngleB = 180 - atan(y2/abs(x2))*180/3.14;
    }
    if( (x2 < 0) && (y2 < 0) ){
        AngleB = 180 + atan(abs(y2)/abs(x2))*180/3.14;
    }
    if( (x2 > 0) && (y2 < 0) ){
        AngleB = 360-atan(abs(y2)/x2)*180/3.14;
    }
    */

    if( (x2 > 0) && (y2 > 0) ){
        //a1
        AngleB = 90- atan(x2/y2)*180/3.14;
        tempp=90-AngleB;
    }
    if( (x2 < 0) && (y2 > 0) ){
        AngleB = 180 - atan(y2/abs(x2))*180/3.14;
        tempp=180-AngleB;
    }
    if( (x2 < 0) && (y2 < 0) ){
        AngleB = 270 - atan(abs(x2)/abs(y2))*180/3.14;
        tempp=270-AngleB;
    }
    if( (x2 > 0) && (y2 < 0) ){
        AngleB = 360-atan(abs(y2)/x2)*180/3.14;
        tempp=360-AngleB;
    }

    if(AngleB > AngleA){
        AngleC =AngleB-AngleA;
    }else{
        AngleC=360-abs(AngleB-AngleA);
    }

}
