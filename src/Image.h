#ifndef IMAGE_H
#define IMAGE_H
#include "dt_core.h"

class CImage
{
public:
    CImage(COMM_CTRL_DESC* desc,CSkinForm* parent);
    //void SetRect(RECT *prc);
    ~CImage();

    bool Attach(CCommCtrl* ctrl);
    virtual int OnImagePaint(HDC hdc);
    virtual int OnEraseBk(HDC hdc);
    int  internal_proc(HWND hwnd, int message, WPARAM w, LPARAM l);
    int  internal_on_paint(HWND hwnd);

protected:
    HWND        m_hwnd;
    int         m_w,m_h,old_height;
private:
    CCommCtrl*  m_ctrl;
    CSkinForm*  m_parent;

    double      m_value,m_prev_vlaue;

    HDC         m_hdc;
    std::string m_text;
    RECT        m_rect;
    char        buf[32];
    WNDPROC     old_wnd_proc;

    //============================
    CStatic*    m_static;
    int width,height,top,left;
    HDC         m_hdcMem;

};

#endif // PERCENT_H
