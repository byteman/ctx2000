#ifndef MAINCTRL_H
#define MAINCTRL_H
#include <Poco/Runnable.h>
#include <Poco/Event.h>
#include <Poco/Thread.h>
#include <Poco/Notification.h>
#include <Poco/Timer.h>
#include "tajidbmgr.h"
#include "dataacquire.h"
#include "diantai.h"
#include "extdev/gpio.h"
using Poco::Event;
using Poco::Runnable;
using Poco::Thread;
using Poco::Timer;
using Poco::Notification;
typedef std::vector<int> TIDList;
typedef struct tag_JDQ{
    bool set_flag;
    int  set_timer;
    int  cur_timer;
}TJDQ;
struct TCollisonData{
    int m_slewing;
    int m_trolley;
};
struct TFallData{
    double m_dist, m_weight,m_angle;
    int    m_fall, m_type;
};
class CDBAdmin;
class QCollisionNotification: public Notification
{
public:
        QCollisionNotification(){};
        QCollisionNotification(int code , int slewing, int trolley)
        {
            m_code = code;
            m_coll.m_slewing = slewing;
            m_coll.m_trolley = trolley;
        }
        QCollisionNotification(int code,double dist,double weight, int fall, double angle,int type)
        {
            m_code         = code;
            m_fall.m_angle = angle;
            m_fall.m_dist  = dist;
            m_fall.m_fall  = fall;
            m_fall.m_type  = type;
            m_fall.m_weight= weight;
        }
        ~QCollisionNotification()
        {
        }
public:
        int           m_code;
        TFallData     m_fall;
        TCollisonData m_coll;

};
class CMainCtrl:public Poco::Runnable
{
public:
    CMainCtrl();
    virtual ~CMainCtrl();
    static CMainCtrl& Get();

    bool Start();
    virtual void run();

    //处理接收到的AD,
    void handle_ad(ADNotification* pNf);
    //处理接收到的电台数据
    void handle_diantai(Poco::Notification* pNf);
    //处理gprs数据
    void handle_gprs(Poco::Notification* pNf);

    void init_tj_data();
    void master_loop();
    void slave_loop();
    void SaveSiteInfo();
    void ReadSiteInfo();
    void SaveTowerCraneInfo();
    void SaveBuildingInfo();
    void onTimer(Timer& timer);
    ControledStatus m_control_state,m_old_ctrl_state;
private:
    EWorkMode m_mode;
    int      m_local_id;
    int      m_main_id;
    int      m_wait_id;
    double    m_angle;
    double    m_dist;
    void    LjService();
    std::string      build_qurey_msg();
    int       MaxNo();
    bool      is_conflcit(int callto_id);
    std::string GetNextID();
    std::string GetAddID();
    void Gather_AD();
    int       ValideTCNum();
    void      CreateDefaultTjParam();
    void      InitBDParam();
    void      WatchNetWork(std::string &MainDevID, bool &AddState);
    void      DripMainNoAndAddNo(std::string &MainNo, std::string &RightNo, std::string &AddNo);
    void      UpdateTCStatus();
    void      dealMainData(std::string &msg);
    void      DistillData(std::string &msg,std::string &ID);
    bool      DealHeightInfo();
    bool      DealWorkSiteInfo();
    void DiveideSettingInfomation(std::string &msg);
    void ReadSetting();
    void SaveDivideInfo();
    void SaveWorksiteInfomation();
    void ad_handle(ADNotification* pNf);
    int  GetValidTjList(std::vector<int> &devlist);
    int  GetConflictTjList(std::vector<int> &devlist);
    void InitAlgoData();
    bool is_circle_interset(double x1, double y1, double x2, double y2, double r1, double r2);
    //=====================
    Poco::Thread m_thread;
    Poco::Event  m_rdyEvt;
    Poco::Event  m_quitEvt;
    volatile bool m_quit;

    Poco::Timestamp lastDateTime,sendInfoTime;
    CGpio m_gpio;
    Poco::Timer* m_timer;
    TJDQ m_jdq[3];
    CDBAdmin* m_dbadmin;

    //bool right_turn_limit,left_turn_limit,turn_brake;
};

#endif // MAINCTRL_H
