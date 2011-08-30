#ifndef __SOFTKEYBOARDFORM_H__
#define __SOFTKEYBOARDFORM_H__

#include "dt_core.h"
#include "SoftKeyboard.h"

#define  SoftKeyboardForm_ITEM_NUM   1 

typedef struct __SoftKeyboardForm{
        HWND hWnd;
        int lx;int ly;int rx;int by;
        DWORD dwExStyle;
        BOOL bModal;
        sem_t wait;
    }_SoftKeyboardForm;

class SoftKeyboardForm:public CSkinForm {
public:
    static void Init();
    HWND CreateFormEx(HWND hWnd,
                    int lx = 10,  int ly = 10,
                    int rx = 339,int by = 180,
                    DWORD dwExStyle = WS_EX_TOPMOST|WS_EX_TOOLWINDOW,
                    BOOL bModal = TRUE);

    static void Close();
    void  SetInputMethod(int method);
    void  Forzen(bool yes){
        m_move_able = yes?false:true;
    }
protected:
    static skin_item_t m_skin_item[SoftKeyboardForm_ITEM_NUM];
	static skin_head_t m_skin_head;
    
    static pthread_t winthread;
    static _SoftKeyboardForm WinInfo;
    static int inputmet;
    CSkinLable* m_status;
    CIcon m_mask1;
    CIcon m_mask2;
    bool  m_move_able;
    void CreateIMEWindow();
    virtual void OnLButtonDown(int x, int y);
    virtual void OnLButtonUp(int x, int y);
    virtual void OnShow();
    virtual void OnMouseMove(int x, int y);
    static void * SoftKeyboardForm_Start(void *data);

    static int Key_Check(int x , int y);
};

#endif
