#ifndef FAST_STATIC_H
#define FAST_STATIC_H
#include "dt_core.h"

class CFastStatic
{
public:
    CFastStatic(COMM_CTRL_DESC* desc,CSkinForm* parent);
    ~CFastStatic();
    bool Attach(CCommCtrl* ctrl);
    bool Create(EAlign align=Align_Left);
    void SetText(double value,gal_pixel bkcolor=0,CFont* font=0);
    void SetText(std::string value,gal_pixel bkcolor=0,CFont* font=0);

private:
    CCommCtrl*  m_ctrl;
    CSkinForm*  m_parent;
    int         m_w,m_h,old_height;
    double      m_value,m_prev_vlaue;
    HWND        m_hwnd;
    HDC         m_hdc,m_hdcMem;
    std::string m_text;
    RECT        m_rect;
    char        buf[32];
    CFont*      Font24;
    UINT        format;
};

#endif // PERCENT_H
