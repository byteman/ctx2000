#ifndef GUINOTIFYER_H
#define GUINOTIFYER_H

#include "dt_core.h"
typedef struct tag_Msg{

}TGuiMsg;
/*
1. 通知GUI app关闭的消息
2. 通讯故障后，通知主界面上信号指示消息
3. 实时参数变化后，更新主界面的消息。
4. 指示周围塔机通讯报警的消息
5. 指示上下左右报警的消息
*/
class TGuiNotifyer
{
public:
    TGuiNotifyer();
    void RegisterWindow(HWND hwnd);
private:
    HWND m_hwnd;
};

#endif // GUINOTIFYER_H
