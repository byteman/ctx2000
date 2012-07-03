#include "guinotifyer.h"
#include <Poco/SingletonHolder.h>

TGuiNotifyer::TGuiNotifyer()
{

}
TGuiNotifyer::~TGuiNotifyer()
{

}
TGuiNotifyer& TGuiNotifyer::Get()
{
    static Poco::SingletonHolder<TGuiNotifyer> sh;
    return *sh.get();
}
/*窗体注册需要接收某个类型的消息*/
bool TGuiNotifyer::RegisterWindow(HWND hwnd,int msgType)
{

    TObserverIterator obs_it = m_objList.find(msgType);
    if(obs_it == m_objList.end())
    {
        m_objList[msgType] = new TWindowList;
        m_objList[msgType]->push_back(hwnd);
        return true;
    }

    TWindowList* wndlist = m_objList[msgType];

    TWindowIterator wnd_it = wndlist->begin();

    for(; wnd_it != wndlist->end(); wnd_it++)
    {
        if(*wnd_it == hwnd)
            return false;
    }

    wndlist->push_back(hwnd);

    return true;

}
bool TGuiNotifyer::UnregisterWindow(HWND hwnd,int msgType)
{
    if(hwnd == HWND_INVALID) return false;
    TObserverIterator obs_it = m_objList.find(msgType);

    if(obs_it == m_objList.end()) return false;


    TWindowList* wndlist = m_objList[msgType];

    wndlist->remove(hwnd);
    return true;
}
/*
工作模块通知GUI的接口
*/
bool TGuiNotifyer::Notify(TGuiMsg* msg)
{
    if(msg == NULL) return false;

    TObserverIterator obs_it = m_objList.find(msg->type);
    if(obs_it == m_objList.end()) return false;

    TWindowList* wndlist = m_objList[msg->type];

    TWindowIterator wnd_it = wndlist->begin();

    HWND hwnd = HWND_INVALID;
    for(; wnd_it != wndlist->end(); wnd_it++)
    {
        hwnd = *wnd_it;
        SendMessage(hwnd, MSG_GUI_NOFITY_MSG, NULL, 0);
    }

    return true;
}

