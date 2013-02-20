#ifndef DATAACQUIRE_H
#define DATAACQUIRE_H
#include "Poco/NotificationCenter.h"
#include "Poco/Observer.h"
#include "Poco/NObserver.h"
#include "Poco/AutoPtr.h"
#include <Poco/NotificationQueue.h>
#include <Poco/AbstractObserver.h>
#include <queue>
#include <iostream>
#include "ctx2000.h"
#include "filter.h"
using Poco::NotificationCenter;
using Poco::Observer;
using Poco::NObserver;
using Poco::Notification;
using Poco::AutoPtr;
using Poco::NotificationQueue;
using Poco::AbstractObserver;

class CDataAcquireWorker1;
class CDataAcquireWorker2;
class CDataParseWorker;
class CDataAcquire
{
public:
    CDataAcquire();
    static CDataAcquire& Get();

    bool Start(std::string path1,std::string path2);
    bool Start();

    bool StartAcquireMcuAD(std::string pathToMCU);
    bool StartAcquireAngleEncoder(std::string pathToEncoder);
	int  output(int port, unsigned char state);
    bool Stop();
    bool Pause(bool yes);
    void addObserver(const AbstractObserver& observer);
    void removeObserver(const AbstractObserver& observer);
private:
    bool Init(std::string path1,std::string path2);
    NotificationCenter m_nc;
    NotificationQueue  m_nq;
    CDataAcquireWorker1* m_aq_work1;
    CDataAcquireWorker2* m_aq_work2;
    CDataParseWorker*    m_ps_work;

};

#endif // DATAACQUIRE_H
