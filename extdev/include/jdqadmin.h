#ifndef JDQADMIN_H
#define JDQADMIN_H
#include "ctx2000.h"
#include "gpio.h"
#include "QtzParam.h"
#include <Poco/Timer.h>


#define JDQ_OPEN  IO_LOW_LEVEL
#define JDQ_CLOSE IO_HIGH_LEVEL

typedef struct tag_JDQ{
    bool set_flag; //是否被控标志
    int  set_timer; //被控总时间
    int  cur_timer; //当前倒数的时间
    int  jdq_num; //被控的继电器编号
}TJDQ;
class CJDQAdmin:public CTX_ModuleCtrl
{
public:
    CJDQAdmin();
    virtual ~CJDQAdmin();
    static CJDQAdmin& Get();
    virtual bool Start();
    virtual bool Stop();
    virtual bool Restart();
    virtual bool Reload();
    void Bypass(bool yes);
    void ControlEx(CTX_JDQ jdq, int delayS);
    void Control(CTX_JDQ jdq,   int state);

    int  Service(CTX_JDQ type, bool state);

private:
    void onTimer(Poco::Timer& timer);
    bool            LoadParam();
    Poco::Timer*    m_timer;
    TJDQ            m_jdq[3];
    int             m_fd_time,m_brake_time;
    CGpio           m_gpio;
    ControledStatus m_control_state;
    ControledStatus m_old_ctrl_state;
    bool            m_bypass;
};

#endif // JDQADMIN_H
