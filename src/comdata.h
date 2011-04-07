#ifndef COMDATA_H
#define COMDATA_H
#include "ctx2000.h"
extern TTjRecord g_TC[21];
//主机编号， 当前塔机编号, 塔机类型名， 当前塔机的序列号
extern  std::string MainMachineID,CurID,TCTypeName,CurSerial;
extern  int     RightNo,AddNo,MaxTCNo;
extern  int     TCArmLen,TCBeilv;
extern  std::string StrTCArmLen, StrTCBeilv;
extern  double  wksp[4][2];
extern  double  czwzb[2][2];
extern  double  YNAngle,VStopDis,VWarnDis,BrakeDis,DangerDis,WarnDis,AddAngle;
extern  Twb     wba[11];
extern  TDividePoint DividePoint[21];
extern  int TcNum,wbNum,DividePointNum,TCTotalNum,fd_time;
extern  unsigned char tcorder[21];
extern  double Trolly_dist_min,Trolly_dist_max,V0;
//实时的ad值
extern  int ad_angle, ad_up_angle, ad_car_dist, ad_weight,ad_fengsu, ad_height;
#endif // COMDATA_H
