#include "jdqadmin.h"
#include "mainctrl.h"
#include <Poco/SingletonHolder.h>
#include <Poco/Timer.h>
#include <Poco/Mutex.h>
#include "comdata.h"
#include "iniFile.h"

using Poco::TimerCallback;
using Poco::FastMutex;

static Poco::FastMutex _mutex;
CJDQAdmin::CJDQAdmin()
{
    m_timer = NULL;
    for(int i = 0; i < 12; i ++)
        m_control_state.b[i] = false;
    m_old_ctrl_state = m_control_state;
    CMainCtrl::Get().RegisterModuleCtrl(this);
    m_bypass = false;
}
CJDQAdmin::~CJDQAdmin()
{

}
#define JDQ_RESET_OUT1  12
#define JDQ_RESET_OUT2  13
void CJDQAdmin::ResetDevice(int dev)
{
    if(dev == 1)
    {
        Control((CTX_JDQ)JDQ_RESET_OUT1,JDQ_OPEN);
        Control((CTX_JDQ)JDQ_RESET_OUT2,JDQ_OPEN);
        Poco::Thread::sleep (1000);
        Control((CTX_JDQ)JDQ_RESET_OUT1,JDQ_CLOSE);
        Control((CTX_JDQ)JDQ_RESET_OUT2,JDQ_CLOSE);
    }

}
CJDQAdmin& CJDQAdmin::Get()
{
    static Poco::SingletonHolder<CJDQAdmin> sh;
    return *sh.get();
}
void CJDQAdmin::Bypass(bool yes)
{
    m_bypass = yes;
    m_gpio.Init ();
}
int  CJDQAdmin::Service(CTX_JDQ type, bool state)
{
        fprintf(stderr,"jdq control %d state=%d\n",type,state?1:0);
        if(m_bypass) return 0;
        if(type == JDQ_RIGHT_CIRCLE_LIMIT)
        {
            if(state) //右回转限制
            {
                Control(JDQ_RIGHT_CIRCLE_LIMIT,JDQ_OPEN);
                ControlEx(JDQ_RIGHT_CIRCLE_LIMIT_FD,fd_time);
            }
            else
            {
                Control(JDQ_RIGHT_CIRCLE_LIMIT,JDQ_CLOSE);
            }
        }else if(type == JDQ_CIRCLE_BREAK)
        {
            if(state) //回转制动
            {
                ControlEx(JDQ_CIRCLE_BREAK,brake_time);
            }
            else Control(JDQ_CIRCLE_BREAK,JDQ_CLOSE);
        }else if(type == JDQ_RIGHT_CIRCLE_LIMIT)
        {
            if(state)//左回转限制
            {
                Control(JDQ_LEFT_CIRCLE_LIMIT,JDQ_OPEN);
                ControlEx(JDQ_LEFT_CIRCLE_LIMIT_FD,fd_time);
            }
            else Control(JDQ_LEFT_CIRCLE_LIMIT,JDQ_CLOSE);
        }else{
            if(state) //小车向外高速限制
            {
               Control(type,JDQ_OPEN);
            }
            else Control(type,JDQ_CLOSE);
        }
        return 1;
}
bool CJDQAdmin::Reload()
{
    return true;
}
void CJDQAdmin::Control(CTX_JDQ jdq, int state)
{
    FastMutex::ScopedLock lock(_mutex);
    if(m_bypass) return;
    m_gpio.Output((int)jdq,(IO_STATUS)state);
}
void CJDQAdmin::ControlEx(CTX_JDQ jdq, int delayS)
{
    FastMutex::ScopedLock lock(_mutex);
    if(m_bypass) return;
    switch(jdq){
        case JDQ_CIRCLE_BREAK:
            m_gpio.Output((int)jdq,JDQ_OPEN);
            m_jdq[0].jdq_num   = JDQ_CIRCLE_BREAK;
            m_jdq[0].set_timer = delayS;
            m_jdq[0].set_flag  = true;
            break;
        case JDQ_LEFT_CIRCLE_LIMIT_FD:
            m_gpio.Output((int)jdq,JDQ_OPEN);
            m_jdq[1].jdq_num   = JDQ_LEFT_CIRCLE_LIMIT_FD;
            m_jdq[1].set_timer = delayS;
            m_jdq[1].set_flag  = true;
            break;
        case JDQ_RIGHT_CIRCLE_LIMIT_FD:
            m_gpio.Output(jdq,JDQ_OPEN);
            m_jdq[2].jdq_num   = JDQ_RIGHT_CIRCLE_LIMIT_FD;
            m_jdq[2].set_timer = delayS;
            m_jdq[2].set_flag  = true;
            break;
         default:
            fprintf(stderr,"jdq_admin can't handle %d\n",jdq);
            break;
    }
}
bool CJDQAdmin::LoadParam()
{

    m_jdq[0].set_timer = fd_time;
    m_jdq[1].set_timer = fd_time;
    m_jdq[2].set_timer = brake_time;
    for(int i =0 ;i <3; i++)
    {
        m_jdq[i].set_flag = false;
        m_jdq[i].set_timer= 0;
        m_jdq[i].cur_timer= 0;
        m_jdq[i].jdq_num  = 0;
    }
    m_jdq[0].jdq_num = JDQ_CIRCLE_BREAK;
    m_jdq[1].jdq_num = JDQ_LEFT_CIRCLE_LIMIT_FD;
    m_jdq[2].jdq_num = JDQ_RIGHT_CIRCLE_LIMIT_FD;
    return true;
}
bool CJDQAdmin::Stop()
{
    if(m_timer){
        m_timer->stop();
        delete m_timer;
        m_timer = NULL;
    }
    return true;
}
bool CJDQAdmin::Start()
{
    LoadParam();
    //m_gpio.CreateVirtualFiles();
    m_gpio.Init();
    //Control(JDQ_RESET_OUT1,JDQ_CLOSE);
    //Control(JDQ_RESET_OUT2,JDQ_CLOSE);
    if(m_timer==NULL)
    {
        m_timer = new Poco::Timer(1000,1000);
        Poco::TimerCallback<CJDQAdmin> tc(*this, &CJDQAdmin::onTimer);
        m_timer->start(tc);
    }else{//100ms后重新启动
        m_timer->restart(100);
    }
    return true;
}
bool CJDQAdmin::Restart()
{
    Stop();
    Start();
    return true;
}
void CJDQAdmin::onTimer(Poco::Timer& timer)
{
    for(int i =0 ;i < 3; i++)
    {
        if(m_jdq[i].set_flag)
        {
            if(m_jdq[i].cur_timer<=0)
            {
                FastMutex::ScopedLock lock(_mutex);
                m_gpio.Output(m_jdq[i].jdq_num,JDQ_CLOSE);
                m_jdq[i].set_flag=false;
            }else{
                m_jdq[i].cur_timer--;
            }
        }
    }
    //std::cout << "Callback called after  milliseconds." << std::endl;
}
