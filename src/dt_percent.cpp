#include "dt_percent.h"
#include <stdio.h>
#include <math.h>


DT_Percent::DT_Percent(COMM_CTRL_DESC* desc,CSkinForm* parent,int direction):
    m_direction(direction)
{
    m_ctrl   = new CStatic(desc,parent);
    m_hwnd   = HWND_INVALID;
    m_parent = parent;
    m_w      = desc->w;
    m_h      = desc->h;
    SetRect(&m_rect,0 , 0, desc->w, desc->h);
    m_bgcolor= PIXEL_lightwhite;
    m_fgcolor= PIXEL_green;
}
bool DT_Percent::HasInit()
{
    return (m_hwnd != HWND_INVALID);
}
void DT_Percent::SetColorPercent(TColorSet &rst)
{
    m_colors.clear();
    for(size_t i = 0; i < rst.size(); i++)
        m_colors.push_back(rst.at(i));
}
bool DT_Percent::Create()
{
    if(m_hwnd == HWND_INVALID)
        m_hwnd =  GetDlgItem(m_parent->m_hWnd,m_ctrl->GetId());

    if(m_hwnd != HWND_INVALID)
    {
        m_hdc   = GetClientDC(m_hwnd);
        m_MemDC = CreateCompatibleDC(m_hdc);
        ReleaseDC(m_hdc);
    }
    else{
        fprintf(stderr,"can't get hwnd\n");
    }

    return (m_hwnd!=HWND_INVALID);
}
DT_Percent::~DT_Percent()
{

}
void DT_Percent::PaintOnDC(HDC hdc,double value)
{
    SetBrushColor(hdc,m_bgcolor);
    FillBox(hdc,0,0,m_w,m_h);

    for(int i = m_colors.size(); i > 0; i--)
    {
        //fprintf(stderr,"[%d] corlor=%d value=%0.2f\n",i-1,m_colors.at(i-1).color,m_colors.at(i-1).value);
        if(m_colors.at(i-1).value <= value)
        {
            //fprintf(stderr,"%d %0.2f %0.2f\n",(i-1),m_colors.at(i-1).value,value);

            m_fgcolor = m_colors.at(i-1).color;
            break;
        }
    }
    SetBrushColor(hdc,m_fgcolor);
    if(m_direction==0)
        FillBox(hdc,0,0,m_w*value,m_h);
    else
        FillBox(hdc,0,m_h-m_h*value,m_w,m_h*value);
}
void DT_Percent::Show(double value)
{
    //if(!HasInit())Create();

    m_percent = value;
    if( fabs(value) > 1.0)
        value = 1.0;

    HDC hdc = GetClientDC(m_hwnd);
    PaintOnDC(m_MemDC,value);
    BitBlt(m_MemDC,0,0,m_w,m_h,hdc,0,0,0);
    ReleaseDC(hdc);

}
