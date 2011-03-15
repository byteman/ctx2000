#ifndef STATUSICON_H
#define STATUSICON_H
#include "dt_core.h"
#include "ctx2000.h"
class CStatusIcon
{
public:
    CStatusIcon(CSkinForm* parent,int num, RECT rt,bool islocal=false);
    CStatusIcon(CSkinForm* parent,int x, int y, int r);
    bool SetStatus(HDC hdc,EDevStatus status);
private:
    HDC  m_hdc;
    CSkinForm* m_parent;
    EDevStatus m_cur_status;
    int m_x,m_y,m_r;
    RECT m_rect;
    int  m_num;
    std::string m_num_str;
    bool m_islocal;
    CFont* font;
    gal_pixel m_font_color;
};

#endif // STATUSICON_H
