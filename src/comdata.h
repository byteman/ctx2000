#ifndef COMDATA_H
#define COMDATA_H
#include "ctx2000.h"
typedef std::vector<int> TID;
typedef struct tag_TBD{
    int     zero_ad;
    int     bd_ad;
    double  bd_k;
    double  start_value;
}TBD;
typedef enum{
    BD_ANGLE=0,
    BD_UP_ANGLE,
    BD_WEIGHT,
    BD_SPEED,
    BD_CAR_DIST,
    BD_HEIGHT
}BD_TYPE;
//标定数据结构
extern TBD g_bd[6];
//有效塔机id列表
extern TID g_valid_tj_list;
//和本机冲突的id列表
extern TID g_conflict_tj_list;
extern TTjRecord g_TC[21];
//主机编号， 当前塔机编号, 塔机类型名， 当前塔机的序列号
extern  std::string MainMachineID,CurID,TCTypeName,CurSerial;
extern  int     RightNo,AddNo,MaxTCNo,g_local_id;
extern  int     TCArmLen,TCBeilv;
extern  std::string StrTCArmLen, StrTCBeilv;
extern  double  wksp[4][2];
extern  double  czwzb[2][2];
extern  double  YNAngle,VStopDis,VWarnDis,BrakeDis,DangerDis,WarnDis,AddAngle;
extern  Twb     wba[11];
extern  TDividePoint DividePoint[21];
extern  int TcNum,wbNum,DividePointNum,TCTotalNum,fd_time,brake_time;
//存放有效的塔机的id序列
extern  unsigned char tcorder[21];
extern  double Trolly_dist_min,Trolly_dist_max,V0;
//实时的ad值
extern  int ad_angle, ad_up_angle, ad_car_dist, ad_weight,ad_fengsu, ad_height;
//分别是 小车幅度 吊钩高度 吊钩物体的重量 角度 风速 动臂仰角
extern  double g_car_dist,g_dg_height,g_dg_weight,g_angle,g_speed,g_up_angle;

extern double g_angle_A,g_angle_B,g_angle_C;
extern std::string g_diantai_com,g_ad_com1,g_ad_com2,g_gprs_com;
extern int encoder_addr;
//common functions
extern int set_thread_title(const char* fmt,... );
extern void calc_angle(double bd[2][2],double x,double y,double &AngleA,double &AngleB,double &AngleC);
#endif // COMDATA_H
