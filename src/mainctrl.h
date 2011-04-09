#ifndef MAINCTRL_H
#define MAINCTRL_H
#include <Poco/Runnable.h>
#include <Poco/Event.h>
#include <Poco/Thread.h>
#include "tajidbmgr.h"
#include "dataacquire.h"
#include "diantai.h"
using Poco::Event;
using Poco::Runnable;
using Poco::Thread;

typedef std::vector<int> TIDList;

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
private:
    EWorkMode m_mode;
    int      m_local_id;
    int      m_main_id;
    int      m_wait_id;
    double    m_angle;
    double    m_dist;
    std::string      build_qurey_msg();
    int       MaxNo();
    bool      is_conflcit(int callto_id);
    std::string GetNextID();
    std::string GetAddID();
    int       ValideTCNum();
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
};

#endif // MAINCTRL_H
