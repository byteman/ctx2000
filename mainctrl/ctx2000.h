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
    PolyDef anchor_coord;    //��ת�궨����
    double  y_angle;            //Y������򱱼��ų��ļн�
    double  alert_dist;         //��ת�������
    double  slowdown_dist;     // ��ת���پ���
    double  brake_dist;        //��ת�ƶ�����
    double  inertia_angle;     //���ԽǶ�
    double  car_low_spd_dist;  //С���ϵ��پ���
    double  car_high_spd_dist; //С���ϸ��پ���
};
//�����ı궨ϵ��
struct TCalcParam{
    int    taji_type;
    int    anchor_zero_ad; //��ת���������AD
    double anchor_k;       //��ת�������궨ϵ��
    int    anchor_cur_ad; //��ת��������ǰAD

    int    min_car_dist_ad; //���ȴ��������AD
    double min_car_dist_k;  //���ȴ������궨ϵ��
    int    car_dist_cur_ad; //���ȴ�������ǰAD
};
/*
������ʵʱ�˶�����
*/
struct TTjRtParam{
    TTjRtParam()
    {
        long_arm_angle = 0;
        carrier_pos    = 0;
        weight         = 0;
    }
    double long_arm_angle; //��ۻ�ת�Ƕ�
    double carrier_pos; //С������
    double weight; //����
    /*
        ����
        ����
        �����߶�
    */
};
struct TTjRecord{
    char Serial[20]; //�����������õ��豸���к�
    bool Valide; //�����Ƿ�����
    Poco::Timestamp InfoTime; //���һ���յ�������ʵʱ������ʱ��
    double   Height; //�����߶�
    double   x;
    double   y;
    double   Angle; //�Ի���Ϊ��λ
    double   AngleSpeed;
    double   LongArmLength;
    double   ShortArmLenght;
    double   Position; //����
    double   Rs;  //��С����
    bool     Dyna; //�Ƕ���ʽ
    double   DLong; //����ͶӰ
    double   DHeight; //���۸߶�+�����߶�
    double   L1; //���۵����Ƕ�85������
    double   L2; //���۵���С�Ƕ�15��
    double   a0; //��۸����µ㵽������ת���ĵľ��룬�����򸺻�0
    double   C2; //���㳤��
    double   CAng; //�̶��н�
    double   DLine; //��������
    double   Dang; //��������
    double   L;
    bool     VNoMove; //С����ֹ��־
    bool     NoMove;  //��ת��ֹ��־
};
//��������
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
//��ʼ��10����������
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
    std::string devserail;   //�豸��� [0/1]
    Coord       pos;        //��������  [0/1]
    int         taji_id;
    int         taji_type;  //���������� 0 ƽ�� 1���� [0/1]
    double      height;     //��ۻ���ƽ��۵ĸ߶�(�����ƽ��[���==ƽ���]�����۵Ļ�����ƽ��۸߶�[0/1]
    double      long_arm_length; //��۳��� == С����������[0/1]
    double      min_car_dist; //С������С����,ֻ��ƽ�������� [С�� long_arm_length][0]
    double      short_arm_length; //ƽ��۳���[0/1]
    int         isown;
    //======����=====
    double      max_up_angle; //���������� [1]
    double      min_up_angle; //�����С���� [1]
    double      a0; //�ṹ��������۽µ�����ת���ĵľ���[1]

    TCalcParam           calc;
    std::vector<PolyDef> zone;
    std::vector<PolyDef> div;
    TWorkSiteParam       ws;
    TTjRtParam           rt;
};

//�����������
struct TZoneParam{
    //std::vector<const PolyDef> zone;
};
//�ֽ籣������ �������빹��һ����·
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

  unsigned short   wHeader;            // ��Ϣͷ 0xaa55
  unsigned short   wType;              //��Ϣ����
  unsigned short   wEvent;             //��Ϣ�¼� ���ؽ��
  unsigned short   wLength;            //��Ϣ��ĳ���
  char             pchContent[1024];         // ��Ϣ��
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
    msg_query,					//��ѯ
    msg_ack,					//ȷ��
    msg_nack,					//ʧ��
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
    char  tj_caller_id; //���ͷ����
    char  tj_callto_id; //���з����
    float angle;       //���ͷ��ĽǶ�
    float dist;         //���ͷ���С������
}CTX_QueryMsg;

typedef struct tag_UploadMsg{
    char  mode_type;
    char  tj_caller_id; //���ͷ����

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
    JDQ_RIGHT_CIRCLE_LIMIT=0,         //1#??�һ�ת����       �Һ�
    JDQ_CIRCLE_BREAK,               //2#??��ת�ƶ�         �ص���Դ[���ƶ�ʱ������]
    JDQ_LEFT_CIRCLE_LIMIT,          //3#??���ת����	      ���
    JDQ_CAR_OUTSIDE_HISPEED_LIMIT,  //4#??С���������и������� �ϻ�
    JDQ_CAR_OUTSIDE_BREAK,          //5#??С����������ͣ��     �Ϻ�
    JDQ_CAR_INSIDE_BREAK,           //6#??С����������ͣ��    �º�
    JDQ_CAR_INSIDE_SPEED_LIMIT,     //7#??С���������и������� �»�
    JDQ_HOOK_UP_LIMIT,              //8#??������������
    JDQ_LEFT_CIRCLE_HISPEED_LIMIT,  //9 #??���ת��������     ���
    JDQ_RIGHT_CIRCLE_HISPEED_LIMIT, //10#??�һ�ת��������     �һ�
    JDQ_LEFT_CIRCLE_LIMIT_FD,       //11#??���ת���Ʒ����̵���[�з���ʱ������]
    JDQ_RIGHT_CIRCLE_LIMIT_FD,       //12#??�һ�ת���Ʒ����̵���[�з���ʱ������]
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
