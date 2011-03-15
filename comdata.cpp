#include "comdata.h"

std::string MainMachineID,CurID,TCTypeName,CurSerial;
int     RightNo,AddNo,MaxTCNo;
double wksp[4][2];
double czwzb[2][2];
double YNAngle,VStopDis,VWarnDis,BrakeDis,DangerDis,WarnDis,AddAngle;
Twb    wba[11];
TDividePoint DividePoint[21];
int TcNum,wbNum,DividePointNum,TCTotalNum,fd_time;
unsigned char tcorder[21];
double Trolly_dist_min,Trolly_dist_max,V0;
