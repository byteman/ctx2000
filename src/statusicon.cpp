#include "statusicon.h"
#include <Poco/Format.h>
struct TDevStatusColor{
    EDevStatus status;
    gal_pixel  color;
};
static  gal_pixel DevStatusColorMaps[] = {
    PIXEL_green,PIXEL_yellow,PIXEL_red
};
static  gal_pixel DevBkColorMaps[] = {
    PIXEL_lightgray,PIXEL_darkblue
};
CStatusIcon::CStatusIcon(CSkinForm* parent,int num, RECT rt,bool islocal):
    m_parent(parent),
    m_rect(rt),
    m_num(num),
    m_islocal(islocal)
{
    m_cur_status = EOK;
    DevStatusColorMaps[0] = PIXEL_green;
    DevStatusColorMaps[1] = PIXEL_yellow;
    DevStatusColorMaps[2] = PIXEL_red;
    DevBkColorMaps[0]     = PIXEL_lightgray;
    DevBkColorMaps[1]     = PIXEL_darkblue;
    m_r = RECTH(rt)/4;
    font = CFontMgr::Get().GetFont("song",16);
    font->setFontColor(PIXEL_black);
    m_num_str = Poco::format("%d",m_num);
    m_font_color = PIXEL_black;
}
CStatusIcon::CStatusIcon(CSkinForm* parent,int x, int y, int r):
    m_parent(parent),
    m_x(x),
    m_y(y),
    m_r(r)
{
    m_cur_status = EOK;
    DevStatusColorMaps[0] = PIXEL_green;
    DevStatusColorMaps[1] = PIXEL_yellow;
    DevStatusColorMaps[2] = PIXEL_red;
}

bool CStatusIcon::SetStatus(HDC hdc,EDevStatus status)
{

    SetBkMode(hdc,BM_TRANSPARENT);
    //gal_pixel oldcolor = SetBrushColor(hdc,DevBkColorMaps[index]);
    //FillEllipse(hdc,m_rect.left, m_rect.right, m_rect.right, m_rect.bottom);
    //FillBox(hdc,m_rect.left, m_rect.top,RECTW(m_rect), RECTH(m_rect));

    SetBrushColor(hdc,DevStatusColorMaps[status]);
    int x = m_rect.left + RECTW(m_rect)/2;
    int y = m_rect.top  + RECTH(m_rect)/2;

    FillCircle(hdc, m_rect.left + m_r, y, m_r);
    RECT fontrect;

    SetRect(&fontrect,x,m_rect.top,m_rect.right,m_rect.bottom);

    m_parent->DrawMyText(hdc,font,&fontrect,m_num_str);


}
