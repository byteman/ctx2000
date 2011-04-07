#include "comdata.h"

std::string MainMachineID,CurID,TCTypeName,CurSerial;
int    RightNo,AddNo,MaxTCNo;
int    TCArmLen,TCBeilv;
std::string StrTCArmLen, StrTCBeilv;
double wksp[4][2];
double czwzb[2][2];
double YNAngle,VStopDis,VWarnDis,BrakeDis,DangerDis,WarnDis,AddAngle;
Twb    wba[11];
TDividePoint DividePoint[21];
int TcNum,wbNum,DividePointNum,TCTotalNum,fd_time;
unsigned char tcorder[21];
double Trolly_dist_min,Trolly_dist_max,V0;
int ad_angle, ad_up_angle, ad_car_dist, ad_weight,ad_fengsu, ad_height;
TTjRecord g_TC[21];
