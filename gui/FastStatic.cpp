#include "FastStatic.h"
#include <stdio.h>

CFastStatic::CFastStatic(COMM_CTRL_DESC* desc,CSkinForm* parent)
{
    //不用自动释放该控件，父窗口会自动释放
    m_ctrl   = new CStatic(desc,parent);
    m_hwnd   = HWND_INVALID;
    m_parent = parent;
    m_w      = desc->w;
    m_h      = desc->h;
    SetRect(&m_rect,0 , 0, desc->w, desc->h);
    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
    format  = DT_WORDBREAK;
}
CFastStatic::~CFastStatic()
{
    if(m_ctrl){

    }
}
bool CFastStatic::Create(EAlign align)
{
    if(m_hwnd == HWND_INVALID)
        m_hwnd =  GetDlgItem(m_parent->m_hWnd,m_ctrl->GetId());

    if(m_hwnd != HWND_INVALID)
    {
        m_hdc   = GetClientDC(m_hwnd);
        m_hdcMem= CreateCompatibleDC(m_hdc);
        ReleaseDC(m_hdc);
    }
    else{
        fprintf(stderr,"can't get hwnd\n");
    }
    switch(align)
    {
        case Align_Center:
            format |= DT_CENTER|DT_VCENTER;
            break;
        case Align_Left:
            format |= DT_LEFT;
            break;
        case Align_Right:
            format |= DT_RIGHT;
            break;
        default:
            format |= DT_LEFT;
            break;
    }
    return (m_hwnd!=HWND_INVALID);
}
bool CFastStatic::Attach(CCommCtrl* ctrl)
{
    m_ctrl = ctrl;

    m_hwnd =  GetDlgItem(m_parent->m_hWnd,m_ctrl->GetId());

    if(m_hwnd != HWND_INVALID)
    {
        m_hdc    = GetClientDC(m_hwnd);
        m_hdcMem = CreateCompatibleDC(m_hdc);
        ReleaseDC(m_hdc);
    }
    else{
        fprintf(stderr,"can't attch hwnd\n");
    }

    return (m_hwnd!=HWND_INVALID);
}

/*
RGB2Pixel(HDC_SCREEN,150,208,209)
*/
void CFastStatic::SetText(std::string value,gal_pixel bkcolor,CFont* font)
{

    if(m_hwnd == HWND_INVALID){
        fprintf(stderr, "CFastStatic should call create first\n");
        return;
    }
    m_hdc = GetClientDC(m_hwnd);

    if(font==NULL)
        SelectFont(m_hdcMem,GetSystemFont(SYSLOGFONT_FIXED));
            //SelectFont(m_hdcMem,Font24->pfont);

    else
        SelectFont(m_hdcMem,font->pfont);
    if(bkcolor==0)bkcolor=RGB2Pixel(HDC_SCREEN,150,208,209);

    SetBkColor(m_hdcMem,bkcolor);
    SetBrushColor(m_hdcMem,bkcolor);
    FillBox(m_hdcMem,m_rect.left,m_rect.top,RECTW(m_rect),RECTH(m_rect));


    DrawText(m_hdcMem,  value.c_str(),value.length (),&m_rect,format);
    BitBlt(m_hdcMem,0,0,m_w,m_h,m_hdc,0,0,0);

    ReleaseDC(m_hdc);
}
void CFastStatic::SetText(double value,gal_pixel bkcolor,CFont* font)
{
    char buf[32] = {0,};
    snprintf(buf, 32, "%0.2f",value);
    SetText(buf,bkcolor,font);
}
