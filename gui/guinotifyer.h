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
1. ֪ͨGUI app�رյ���Ϣ
2. ͨѶ���Ϻ�֪ͨ���������ź�ָʾ��Ϣ
3. ʵʱ�����仯�󣬸������������Ϣ��
4. ָʾ��Χ����ͨѶ��������Ϣ
5. ָʾ�������ұ�������Ϣ
6. ��̨�н��յ���������Ҫ����������ʾ
*/
class TGuiNotifyer
{
public:
    TGuiNotifyer();
    ~TGuiNotifyer();
    static TGuiNotifyer& Get();
    /*ĳ��������Ҫ����ĳ�����͵���Ϣ*/
    bool RegisterWindow(HWND hwnd,int msgType);
    bool UnregisterWindow(HWND hwnd,int msgType);
    bool Notify(TGuiMsg* msg);
private:
    HWND m_hwnd;
    TGuiMsgList     m_GuiMsgList;
    TObserverList   m_objList;
};

#endif // GUINOTIFYER_H
