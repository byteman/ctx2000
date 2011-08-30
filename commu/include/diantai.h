#ifndef DIANTAI_H
#define DIANTAI_H
#include "Poco/NotificationCenter.h"
#include "Poco/Observer.h"
#include "Poco/NObserver.h"
#include "Poco/AutoPtr.h"
#include <Poco/NotificationQueue.h>
#include <Poco/AbstractObserver.h>
#include <Poco/Timestamp.h>
#include <queue>
#include <iostream>
#include "ctx2000.h"
using Poco::NotificationCenter;
using Poco::Observer;
using Poco::NObserver;
using Poco::Notification;
using Poco::AutoPtr;
using Poco::NotificationQueue;
using Poco::AbstractObserver;

class CDianTaiDataReceiver;
class CDianTaiDataSender;
class CDianTaiDataParseWorker;
class CDianTai
{
public:
    CDianTai();
    static CDianTai& Get();
    bool Start(std::string path);
    bool Stop();
    int  GetMessage(CTX_Message& msg,int type=1, int timeout_ms=1000);
    size_t  CheckMessage(int timeout_ms=1000,int type=1);
    int  SendMessage(CTX_Message& msg, int timeout_ms=1000);
    int  SendMessage(std::string& msg, int timeout_ms=1000);
    void ClearMessage();
    void addObserver(const AbstractObserver& observer);
    void removeObserver(const AbstractObserver& observer);
    bool  haveSignal();
    int  getSignalDB();
    unsigned int GetReceivedCount();
    void         ResetCount();
private:

    bool Init(std::string path);
    NotificationCenter m_nc;
    NotificationQueue  m_nq;
    CDianTaiDataReceiver*       m_recv_worker;
    CDianTaiDataParseWorker*    m_ps_worker;
    CDianTaiDataSender*         m_send_worker;

};

#endif // DIANTAI_H
