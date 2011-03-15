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
using Poco::NotificationCenter;
using Poco::Observer;
using Poco::NObserver;
using Poco::Notification;
using Poco::AutoPtr;
using Poco::NotificationQueue;
using Poco::AbstractObserver;

struct TAD{
    TAD()
    {
        m_type = -1;
        m_value = 0;
    }
    TAD(char type, short value):
        m_type(type),
        m_value(value)
    {}
    char   m_type;
    int    m_value;
};
typedef std::queue<TAD> TAdQueue;
class ADNotification: public Notification
{
public:
    ADNotification(TAdQueue *queue):
        m_queue(queue)
    {

    }
    virtual ~ADNotification()
    {
        std::cerr << "deconstruct ADNotification\n";
    }
    TAdQueue* Message() const
    {
        return m_queue;
    }
private:
    TAdQueue *m_queue;
};
class CDataAcquireWorker1;
class CDataAcquireWorker2;
class CDataParseWorker;
class CDataAcquire
{
public:
    CDataAcquire();
    static CDataAcquire& Get();

    bool Start(std::string path);
    bool Stop();
    bool Pause(bool yes);
    void addObserver(const AbstractObserver& observer);
    void removeObserver(const AbstractObserver& observer);
private:
    bool Init(std::string path);
    NotificationCenter m_nc;
    NotificationQueue  m_nq;
    CDataAcquireWorker1* m_aq_work1;
    CDataAcquireWorker2* m_aq_work2;
    CDataParseWorker*    m_ps_work;
};

#endif // DATAACQUIRE_H
