#ifndef COMDATA_H
#define COMDATA_H
#include "ctx2000.h"
typedef std::vector<int> TID;
typedef struct tag_TBD{
    bool    valid;
    int     zero_ad;
    int     bd_ad;
    double  bd_k;
    double  start_value;
}TBD;
typedef enum{
    BD_ANGLE=0,
    BD_UP_ANGLE,
    BD_WEIGHT,
    BD_WEIGHT2,
    BD_WEIGHT3,
    BD_SPEED,
    BD_CAR_DIST,
    BD_HEIGHT,
    BD_ANGLE_X,
    BD_ANGLE_Y,
    BD_MAX
}BD_TYPE;
#define ctx2000_cfg "./etc/ctx2000.ini"
#define ctx2000_protect_data "./etc/protect.red"
#define ctx2000_db  "./etc/ctx2000.sqlite3"
#define ctx2000_torque_db "./etc/tc.db"
#define ctx2000_tc_data "./etc/tc.red"
#define DBG_POS fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
//궨ݽṹ
extern TBD g_bd[13];
//Чidб
extern TID g_valid_tj_list;
//ͱͻidб
extern TID g_conflict_tj_list;
extern TTjRecord g_TC[21];
//ţ ǰ,  ǰк
extern  std::string MainMachineID,CurID,TCTypeName,CurSerial;
extern  int     RightNo,AddNo,MaxTCNo,g_local_id;
extern  int     TCArmLen,g_tc_rate;
extern  int     gMainMenuIndex;
extern  std::string StrTCArmLen, StrTCRate;
extern  double  wksp[4][2];
extern  double  czwzb[2][2];
extern  double  YNAngle,VStopDis,VWarnDis,BrakeDis,DangerDis,WarnDis,AddAngle;
extern  int     FDTime,BrakeTime;
extern  Twb     wba[11];
extern  TDividePoint DividePoint[21];
extern  int TcNum,wbNum,DividePointNum,TCTotalNum,fd_time,brake_time;
//Чid
extern  unsigned char tcorder[21];
extern  double Trolly_dist_min,Trolly_dist_max,V0;
//ʵʱadֵ
extern  int ad_angle, ad_up_angle, ad_car_dist, ad_weight,ad_angle_x,ad_angle_y,ad_wild_speed, ad_height;
//ֱ С ߶  Ƕ  
extern  double g_car_dist,g_dg_height,g_dg_weight,g_angle,g_wild_speed,g_up_angle,g_angle_x,g_angle_y;
extern  double g_rated_weight;
extern double g_angle_A,g_angle_B,g_angle_C;
extern std::string g_diantai_com,g_ad_com,g_encoder_com,g_gprs_com;
extern int encoder_addr;
//======================gprs data====================
extern std::string gprs_remote_ip;
extern std::string gprs_dtu_id;
extern int gprs_remote_port;
extern bool g_show_up_angle;
extern bool g_show_speed;
extern bool g_show_dg_height;
extern bool g_show_max_weight;
extern int  g_alarm_wild_speed;
//======================ЩҪĲ,ҲеĹ̶======================
extern double          tmp_wksp[4][2]; //64
extern double          tmp_czwzb[2][2]; //32
extern double          tmp_YNAngle, tmp_VStopDis,tmp_VWarnDis,tmp_BrakeDis,tmp_DangerDis; //48
extern double          tmp_WarnDis,tmp_AddAngle; //24
extern TTjRecord       tmp_TC[21]; //200*21=4000k
extern TDividePoint    tmp_DividePoint[21]; //16x21
extern Twb             tmp_wba[11];
extern int             tmp_TcNum,tmp_wbNum,tmp_DividePointNum;
//=====================================================================================
//common functions
extern int set_thread_title(const char* fmt,... );
extern void calc_angle(double bd[2][2],double x,double y,double &AngleA,double &AngleB,double &AngleC);
#endif // COMDATA_H
