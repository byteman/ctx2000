#ifndef COMDATA_H
#define COMDATA_H
#include "ctx2000.h"
extern TTjRecord g_TC[21];
//������ţ� ��ǰ�������, ������������ ��ǰ���������к�
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
//ʵʱ��adֵ
extern  int ad_angle, ad_up_angle, ad_car_dist, ad_weight,ad_fengsu, ad_height;
#endif // COMDATA_H
