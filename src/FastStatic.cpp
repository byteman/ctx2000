#include "FastStatic.h"
#include <stdio.h>

CFastStatic::CFastStatic(COMM_CTRL_DESC* desc,CSkinForm* parent)
{
    m_w     = desc->w;
    m_h     = desc->h;
    m_hwnd  = HWND_INVALID;
    m_parent= parent;
    SetRect(&m_rect,0,0,m_w,m_h);
    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
}

static int m_ctrl_proc(HWND hwnd, int message, WPARAM w, LPARAM l)
{
    CFastStatic  *myptr = (CFastStatic*)GetWindowAdditionalData2(hwnd);
    if(myptr)
        return myptr->internal_proc(hwnd,message,w,l);
}
int  CFastStatic::internal_on_paint(HWND hwnd)
{

    HDC hdc = BeginPaint(hwnd);

    snprintf(buf,32,"%0.1f",m_value);

    SetBkMode(hdc,BM_TRANSPARENT);
    SelectFont(hdc,GetSystemFont(SYSLOGFONT_FIXED));
    DrawText(hdc,buf,strlen(buf),&m_rect,DT_TOP|DT_CENTER);

    EndPaint(hwnd,hdc);
}
int  CFastStatic::internal_proc(HWND hwnd, int message, WPARAM w, LPARAM l)
{

    if( (message == MSG_PAINT) && (hwnd == m_hwnd))
    {
        return internal_on_paint(hwnd);
    }else{
        return old_wnd_proc(hwnd,message,w,l);
    }



}
bool CFastStatic::Attach(CCommCtrl* ctrl)
{
    m_ctrl  = ctrl;



    m_hwnd =  GetDlgItem(m_parent->m_hWnd,m_ctrl->GetId());

    if(m_hwnd != HWND_INVALID)
    {
        //SetWindowAdditionalData2(m_hwnd,(DWORD)this);
        //old_wnd_proc = SetWindowCallbackProc(m_hwnd,m_ctrl_proc);
        HDC hdc = GetClientDC(m_hwnd);
        m_hdcMem = CreateCompatibleDC(hdc);
        ReleaseDC(hdc);
    }
    else{
        fprintf(stderr,"can't attch hwnd\n");
    }

    return (m_hwnd!=HWND_INVALID);
}
CFastStatic::~CFastStatic()
{

}
/*
RGB2Pixel(HDC_SCREEN,150,208,209)
*/
void CFastStatic::SetText(std::string value,gal_pixel bkcolor,CFont* font)
{
    m_hdc = GetClientDC(m_hwnd);

    if(font==NULL)
        SelectFont(m_hdcMem,GetSystemFont(SYSLOGFONT_FIXED));
    else
        SelectFont(m_hdcMem,font->pfont);
    if(bkcolor==0)bkcolor=RGB2Pixel(HDC_SCREEN,150,208,209);

    SetBkColor(m_hdcMem,bkcolor);
    SetBrushColor(m_hdcMem,bkcolor);
    FillBox(m_hdcMem,m_rect.left,m_rect.top,RECTW(m_rect),RECTH(m_rect));
    DrawText(m_hdcMem,  value.c_str(),value.length (),&m_rect,DT_LEFT|DT_TOP);
    BitBlt(m_hdcMem,0,0,m_w,m_h,m_hdc,0,0,0);
    ReleaseDC(m_hdc);
}
void CFastStatic::SetText(double value)
{
    m_value = value;

    if(m_value == m_prev_vlaue)
    {
        return ;
    }
    m_prev_vlaue = m_value;

    UpdateWindow(m_hwnd,FALSE);

}
