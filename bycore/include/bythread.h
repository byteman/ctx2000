#ifndef BYTHREAD_H
#define BYTHREAD_H
#include <Poco/Thread.h>

class ByThread:public Poco::Runnable
{
public:
    virtual ~ByThread(){}
    bool start(long timeout_ms=1000);
    bool stop(long timeout_ms=1000);
    bool set_thread_name(std::string th_name);
    virtual bool before_run();
    virtual void service();
    virtual void run();
protected:
    Poco::Thread  m_thread;
    Poco::Event   m_rdyEvt;
    Poco::Event   m_quitEvt;
    bool          m_quit;
};

#endif // BYTHREAD_H
