#include "bythread.h"

bool ByThread::start(long timeout_ms)
{
   m_quit = false;
   m_thread.start(*this);
   return m_rdyEvt.tryWait(timeout_ms);
}
bool ByThread::stop(long timeout_ms)
{
    m_quit = true;
    return m_quitEvt.tryWait(timeout_ms);
}
void ByThread::service()
{

}
bool ByThread::before_run()
{
    return true;
}
void ByThread::run()
{
    m_rdyEvt.set();
    m_quitEvt.reset();
    m_quit = false;
    if(before_run())
    {
        while(!m_quit)
        {
            service();
        }
    }
    m_quit = true;
    m_quitEvt.set();
}
