#ifndef PERCENT_H
#define PERCENT_H
#include "dt_core.h"

class CPercent
{
public:
    CPercent();
    ~CPercent();
    bool Init(CSkinForm* parent,CStatic* ctrl,int w, int h);
    int  worksite_proc(HWND hwnd, int message, WPARAM w, LPARAM l);
    void Show(double percent);
private:
    CStatic* m_ctrl;
    CSkinForm* m_parent;
    int      m_w,m_h,old_height;
    double m_percent;
    HWND m_hwnd;
    HDC  m_hdc;
    std::string m_text;
    RECT m_rect;
    bool flag;

};

#endif // PERCENT_H
