#ifndef MAINCTRL_H
#define MAINCTRL_H
#include <Poco/Runnable.h>
#include <Poco/Event.h>
#include <Poco/Thread.h>
#include <Poco/Notification.h>
#include <Poco/Timer.h>
#include <Poco/SharedMemory.h>
#include "tajidbmgr.h"
#include "dataacquire.h"
#include "diantai.h"
#include "dt_core.h"
#include "gprs.h"
using Poco::Event;
using Poco::Runnable;
using Poco::Thread;
using Poco::Timer;
using Poco::Notification;
typedef std::vector<CTX_ModuleCtrl*> TModulesList;
typedef std::vector<int> TIDList;
typedef std::vector<std::string>  StringList;
struct TCollisonData{
    int m_slewing;
    int m_trolley;
};
struct TFallData{
    double m_dist, m_weight,m_angle,m_speed;
    int    m_fall, m_type;
};
#define MSG_CONTRL_MSG  MSG_USER+0x100
#define DebugMsg(msg) std::cerr << msg << "\n"

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
        QCollisionNotification(int code,double dist,double weight, int fall, double angle,double speed,int type)
        {
            m_code         = code;
            m_fall.m_angle = angle;
            m_fall.m_dist  = dist;
            m_fall.m_fall  = fall;
            m_fall.m_type  = type;
            m_fall.m_weight= weight;
            m_fall.m_speed = speed;
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
    bool mainctrl_dev_ctrl_func(int state);
    bool Start();
    bool Stop();
    virtual void run();
    bool RegisterModuleCtrl(CTX_ModuleCtrl* obj);
    void init_tj_data();
    void master_loop();
    void slave_loop();
    void SaveSiteInfo();
    void ReadSiteInfo();
    void SaveTowerCraneInfo();
    void SaveBuildingInfo();
    void onTimer(Timer& timer);
    EWorkMode GetDevMode();
    bool      NotifyBypass(bool on);
    void    SignalMode();
    bool    InitSharedMem();
    void    SetSharedata();
    ControledStatus m_control_state,m_old_ctrl_state;
private:
    Poco::SharedMemory m_shared;
    EWorkMode m_mode;
    int      m_local_id;
    int      m_main_id;
    int      m_wait_id;
    double    m_angle;
    double    m_dist;
    void    LjService();
    void    WildService();
    void    DiantaiService();
    std::string      build_qurey_msg();
    int       MaxNo();
    bool      is_conflcit(int callto_id);
    std::string GetNextID();
    std::string GetAddID();
    void      CollideDetect();
    void      ParseCollideStatus();
    int       ValideTCNum();
    void      CreateDefaultTjParam();
    void      InitBDParam();
    void      SendWetRecord(double w,bool alarm);
    void      SendAlarmData();
    void      WatchNetWork(std::string &MainDevID, bool &AddState);
    void      DripMainNoAndAddNo(std::string &MainNo, std::string &RightNo, std::string &AddNo);
    void      UpdateTCStatus();
    void      dealMainData(std::string &msg);
    void      DistillData(std::string &msg,std::string &ID);
    bool      DealHeightInfo();
    bool      DealWorkSiteInfo();
    bool      DiveideSettingInfomation(std::string &msg);
    bool      ReloadParam();
    void SaveDivideInfo();
    void SaveWorksiteInfomation();
    int  GetValidTjList(std::vector<int> &devlist);
    int  GetConflictTjList(std::vector<int> &devlist);
    void InitAlgoData();
    void PushErrorMsg(std::string msg);
    void PushWarnMsg(std::string  msg);

    bool is_circle_interset(double x1, double y1, double x2, double y2, double r1, double r2);
    //=====================
    Poco::Thread m_thread;
    Poco::Event  m_rdyEvt;
    Poco::Event  m_quitEvt;
    volatile bool m_quit;

    Poco::Timestamp lastDateTime,sendInfoTime;


    CDBAdmin*  m_dbadmin;
    TModulesList m_ModuleList;
public:
    StringList m_errList;
    StringList m_warnList;
    //bool right_turn_limit,left_turn_limit,turn_brake;
};

#endif // MAINCTRL_H
