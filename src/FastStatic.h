#ifndef FAST_STATIC_H
#define FAST_STATIC_H
#include "dt_core.h"

class CFastStatic
{
public:
    CFastStatic(COMM_CTRL_DESC* desc,CSkinForm* parent);
    ~CFastStatic();
    bool Attach(CCommCtrl* ctrl);
    int  internal_proc(HWND hwnd, int message, WPARAM w, LPARAM l);
    int  internal_on_paint(HWND hwnd);
    void SetText(double value);
    void SetText(std::string value);


private:
    CCommCtrl*  m_ctrl;
    CSkinForm*  m_parent;
    int         m_w,m_h,old_height;
    double      m_value,m_prev_vlaue;
    HWND        m_hwnd;
    HDC         m_hdc;
    std::string m_text;
    RECT        m_rect;
    char        buf[32];
    WNDPROC     old_wnd_proc;

};

#endif // PERCENT_H
