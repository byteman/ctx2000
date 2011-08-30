#ifndef GUINOTIFYER_H
#define GUINOTIFYER_H

#include "dt_core.h"
#include <queue>
#include <list>
#include <map>
typedef struct tag_Msg{
    tag_Msg(int ty, std::string msg="GuiNotify", void* d=NULL)
    {
        type    = ty;
        strMsg  = msg;
        data    = d ;
    }
    int     type;
    void*   data;
    std::string strMsg;
}TGuiMsg;

#define MSG_GUI_NOFITY_MSG  (MSG_USER + 0x101)
typedef std::queue<TGuiMsg>             TGuiMsgList;
typedef std::list<HWND>                 TWindowList;
typedef TWindowList::const_iterator     TWindowIterator;
typedef std::map<int, TWindowList*>     TObserverList;
typedef TObserverList::const_iterator   TObserverIterator;
/*
1. 通知GUI app关闭的消息
2. 通讯故障后，通知主界面上信号指示消息
3. 实时参数变化后，更新主界面的消息。
4. 指示周围塔机通讯报警的消息
5. 指示上下左右报警的消息
6. 电台中接收到的数据需要在主界面显示
*/
class TGuiNotifyer
{
public:
    TGuiNotifyer();
    ~TGuiNotifyer();
    static TGuiNotifyer& Get();
    /*某个窗体需要接收某个类型的消息*/
    bool RegisterWindow(HWND hwnd,int msgType);
    bool UnregisterWindow(HWND hwnd,int msgType);
    bool Notify(TGuiMsg* msg);
private:
    HWND m_hwnd;
    TGuiMsgList     m_GuiMsgList;
    TObserverList   m_objList;
};

#endif // GUINOTIFYER_H
