#include "comdata.h"
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



void calc_angle(double bd[2][2],double x,double y,double &AngleA,double &AngleB,double &AngleC)
{
    double tempp;
    double x1 = bd[0][0] - x;
    double y1 = bd[0][1] - y;

    double x2 = bd[1][0] - x;
    double y2 = bd[1][1] - y;

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
        AngleA = 180 - atan(y1/abs(x1))*180/3.14;
    }
    if( (x1 < 0) && (y1 < 0) ){
        AngleA = 180 + atan(abs(y1)/abs(x1))*180/3.14;
    }
    if( (x1 > 0) && (y1 < 0) ){
        AngleA = 360-atan(abs(y1)/x1)*180/3.14;
    }

    if( (x2 > 0) && (y2 > 0) ){
        //a1
        AngleB = 90- atan(x2/y2)*180/3.14;
        tempp=90-AngleB;
    }
    if( (x2 < 0) && (y2 > 0) ){
        AngelB = 180 - atan(y2/abs(x2))*180/3.14;
        tempp=180-AngelB;
    }
    if( (x2 < 0) && (y2 < 0) ){
        AngleB = 270 - atan(abs(x2)/abs(y2))*180/3.14;
        tempp=270-AngelB;
    }
    if( (x2 > 0) && (y2 < 0) ){
        AngleA = 360-atan(abs(y2)/x2)*180/3.14;
        tempp=360-AngelB;
    }

    if(AngleB > AngleA){
        AngleC =AngleB-AngleA;
    }else{
        AngleC=360-abs(AngleB-AngleA);
    }

}
