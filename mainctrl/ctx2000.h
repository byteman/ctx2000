#ifndef CTX2000_H
#define CTX2000_H
#include "collalgo.h"
#include <string>
#include <stdint.h>
#include <Poco/Timestamp.h>
using Poco::Timestamp;
#ifdef DEBUG
#define DBG(fmt...) fprintf(stderr,fmt);
#else
#define DBG(fmt...) do { } while (0)
#endif
typedef enum{
    EOK,
    EWARNING,
    EALARM
}EDevStatus;

struct T_taji{
    int num;
    int long_arm_length;
    int short_arm_length;

};
struct TWorkSiteParam{
    TWorkSiteParam()
    {
        for(int i = 0; i < 4; i++)
        {
            Coord pos(0,0);
            worksite_coord.push_back(pos);
        }
        for(int i = 0; i < 2; i++)
        {
            Coord pos(0,0);
            anchor_coord.push_back(pos);
        }
        y_angle = 0;
        alert_dist = 0;
        slowdown_dist = 0;
        brake_dist = 0 ;
        inertia_angle =0;
        car_low_spd_dist = 0;
        car_high_spd_dist = 0;
    }
    void clear()
    {
        worksite_coord.clear();
        anchor_coord.clear();
    }
    PolyDef worksite_coord;  //leftdown-rightdown-rightup-leftup
    PolyDef anchor_coord;    //回转标定坐标
    double  y_angle;            //Y轴与地球北极磁场的夹角
    double  alert_dist;         //回转警告距离
    double  slowdown_dist;     // 回转减速距离
    double  brake_dist;        //回转制动距离
    double  inertia_angle;     //惯性角度
    double  car_low_spd_dist;  //小车断低速距离
    double  car_high_spd_dist; //小车断高速距离
};
//塔机的标定系数
struct TCalcParam{
    int    taji_type;
    int    anchor_zero_ad; //回转传感器零点AD
    double anchor_k;       //回转传感器标定系数
    int    anchor_cur_ad; //回转传感器当前AD

    int    min_car_dist_ad; //幅度传感器零点AD
    double min_car_dist_k;  //幅度传感器标定系数
    int    car_dist_cur_ad; //幅度传感器当前AD
};
/*
塔机的实时运动参数
*/
struct TTjRtParam{
    TTjRtParam()
    {
        long_arm_angle = 0;
        carrier_pos    = 0;
        weight         = 0;
    }
    double long_arm_angle; //大臂回转角度
    double carrier_pos; //小车幅度
    double weight; //重量
    /*
        风速
        倍率
        吊钩高度
    */
};
struct TTjRecord{
    char Serial[20]; //塔机出厂设置的设备序列号
    bool Valide; //塔机是否在线
    Poco::Timestamp InfoTime; //最后一次收到该塔机实时参数的时间
    double   Height; //塔机高度
    double   x;
    double   y;
    double   Angle; //以弧度为单位
    double   AngleSpeed;
    double   LongArmLength;
    double   ShortArmLenght;
    double   Position; //幅度
    double   Rs;  //最小幅度
    bool     Dyna; //是动臂式
    double   DLong; //动臂投影
    double   DHeight; //动臂高度+吊塔高度
    double   L1; //动臂的最大角度85度左右
    double   L2; //动臂的最小角度15度
    double   a0; //大臂根部铰点到塔机回转中心的距离，可正或负或0
    double   C2; //吊点长度
    double   CAng; //固定夹角
    double   DLine; //拉绳长度
    double   Dang; //动臂仰角
    double   L;
    bool     VNoMove; //小车禁止标志
    bool     NoMove;  //回转禁止标志
};
//塔机参数
struct TTjParam{
    TTjParam()
    {
        set_default_param();
    }
    void set_default_param()
    {
        devserail = "notset";
        taji_id = 0;
        pos.x = 0;
        pos.y = 0;
        taji_type = 0;
        height = 0;
        long_arm_length = 0;
        min_car_dist = 0;
        short_arm_length = 0;
        max_up_angle = 0;
        min_up_angle = 0;
        a0 = 0;
        isown = 0;
        PolyDef poly;
//初始化10个保护区域
        for(int j = 0; j < 10; j++)
        {
            for(int i = 0; i < 6; i++)
            {
                Coord pos(0,0);
                poly.push_back(pos);
            }
            zone.push_back(poly);
        }
    }
    void set_worksite_param(TWorkSiteParam* par)
    {
        ws = *par;
    }
    std::string devserail;   //设备编号 [0/1]
    Coord       pos;        //塔机坐标  [0/1]
    int         taji_id;
    int         taji_type;  //塔机的类型 0 平臂 1动臂 [0/1]
    double      height;     //大臂或则平衡臂的高度(如果是平臂[大臂==平衡臂]，动臂的话就是平衡臂高度[0/1]
    double      long_arm_length; //大臂长度 == 小车的最大幅度[0/1]
    double      min_car_dist; //小车的最小幅度,只对平臂有意义 [小于 long_arm_length][0]
    double      short_arm_length; //平衡臂长度[0/1]
    int         isown;
    //======动臂=====
    double      max_up_angle; //大臂最大仰角 [1]
    double      min_up_angle; //大臂最小仰角 [1]
    double      a0; //结构参数，大臂铰点至回转中心的距离[1]

    TCalcParam           calc;
    std::vector<PolyDef> zone;
    std::vector<PolyDef> div;
    TWorkSiteParam       ws;
    TTjRtParam           rt;
};

//保护区域参数
struct TZoneParam{
    //std::vector<const PolyDef> zone;
};
//分界保护参数 坐标点必须构成一个回路
struct TDividingParam{
    //std::vector<const PolyDef> div;
};

#include "stdio.h"
struct CTX_Message{
  CTX_Message()
  {
      wHeader = 0;
      wType   = 0;
      wEvent  = 0;
      wLength = 0;
  }
  /*
    space s u m : x y [7bytes]
 */
  bool isValid()
  {
     unsigned char sum = 0;
     if(wLength < 7)
         return false;
     for(int i = 0; i < wLength-7; i++)
     {
        sum+=pchContent[i];
     }
     char buf1[3] = {0,};
     buf1[0] = pchContent[wLength-2];
     buf1[1] = pchContent[wLength-1];
     char buf2[3] = {0,};
     sprintf(buf2,"%02x",sum);
     //fprintf(stderr,"sum1=%s sum2=%02x",buf,sum);
     //return (sum==pchContent[wLength-1]);
     return ( (buf1[0]==buf2[0]) && (buf1[1]==buf2[1]));
  }

  unsigned short   wHeader;            // 消息头 0xaa55
  unsigned short   wType;              //消息类型
  unsigned short   wEvent;             //消息事件 返回结果
  unsigned short   wLength;            //消息体的长度
  char             pchContent[1024];         // 消息体
  std::string      context;
};
typedef enum{
  UPLOAD_TJ_PARAM,
  UPLOAD_WORKSITE,
  UPLOAD_ZONE,
  UPLOAD_DIV,
  UPLOAD_RT_TJ,
  QRY_TJ_RT_INFO,
  QRY_UPLOAD_REQ
}CTX_Upload_Type;
typedef enum
{
    msg_query,					//查询
    msg_ack,					//确认
    msg_nack,					//失败
} CTX_MsgEvents;

struct TAlarmInfo{
    int         index;
    int         tj_id;
    int         dist_alarm;
    int         angle_alarm;
    std::string datetime;
};

typedef enum{
    mode_slave,
    mode_master,
    mode_unknown
}EWorkMode;

typedef struct tag_QueryMsg{
    char  mode_type;
    char  tj_caller_id; //发送方编号
    char  tj_callto_id; //被叫方编号
    float angle;       //发送方的角度
    float dist;         //发送方的小车幅度
}CTX_QueryMsg;

typedef struct tag_UploadMsg{
    char  mode_type;
    char  tj_caller_id; //发送方编号

}CTX_UpoadMsg;

typedef struct tag_TWB{
    double Pointxy[6][2];
    double h;
    int    VertexNum;
}Twb;
typedef struct tag_Point{
    double x;
    double y;
}TDividePoint;

typedef enum{
    JDQ_RIGHT_CIRCLE_LIMIT=0,         //1#??右回转限制       右红
    JDQ_CIRCLE_BREAK,               //2#??回转制动         关掉电源[有制动时间限制]
    JDQ_LEFT_CIRCLE_LIMIT,          //3#??左回转限制	      左红
    JDQ_CAR_OUTSIDE_HISPEED_LIMIT,  //4#??小车向外运行高速限制 上黄
    JDQ_CAR_OUTSIDE_BREAK,          //5#??小车向外运行停车     上红
    JDQ_CAR_INSIDE_BREAK,           //6#??小车向内运行停车    下红
    JDQ_CAR_INSIDE_SPEED_LIMIT,     //7#??小车向内运行高速限制 下黄
    JDQ_HOOK_UP_LIMIT,              //8#??吊钩起升限制
    JDQ_LEFT_CIRCLE_HISPEED_LIMIT,  //9 #??左回转高速限制     左黄
    JDQ_RIGHT_CIRCLE_HISPEED_LIMIT, //10#??右回转高速限制     右黄
    JDQ_LEFT_CIRCLE_LIMIT_FD,       //11#??左回转限制反顶继电器[有反顶时间限制]
    JDQ_RIGHT_CIRCLE_LIMIT_FD,       //12#??右回转限制反顶继电器[有反顶时间限制]
    jDQ_MAX_NUM
}CTX_JDQ;

struct CTX_ModuleCtrl{
    virtual ~CTX_ModuleCtrl(){}
    virtual bool Restart() = 0;
    virtual bool Reload()  = 0;
    virtual bool Start()   = 0;
    virtual bool Stop()    = 0;
};
struct CTX_ControlNofifyer{
    virtual void OnCollChange(int type, int state);
};
#define CTX_Message_Length (sizeof(CTX_Message) - 4)

#define Notifyer(OBJ,EVT,Callback)  \
    Observer<OBJ, EVT>(*this, &OBJ::Callback)


#endif // CTX2000_H
