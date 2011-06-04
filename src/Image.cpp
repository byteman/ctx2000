#include "Image.h"
#include <stdio.h>

CImage::CImage(COMM_CTRL_DESC* desc,CSkinForm* parent)
{
    m_w     = desc->w;
    m_h     = desc->h;
    m_hwnd  = HWND_INVALID;
    m_parent= parent;
    SetRect(&m_rect,0,0,m_w,m_h);
}

static int m_ctrl_proc(HWND hwnd, int message, WPARAM w, LPARAM l)
{
    CImage  *myptr = (CImage*)GetWindowAdditionalData2(hwnd);
    if(myptr)
        return myptr->internal_proc(hwnd,message,w,l);
}
int CImage::OnImagePaint(HDC hdc)
{
    return 0;
}
int CImage::OnEraseBk(HDC hdc)
{

}
int  CImage::internal_on_paint(HWND hwnd)
{

    //int ret = OnImagePaint(m_hdcMem);
    //int ret = OnImagePaint(m_hdcMem);
    HDC hdc = BeginPaint(hwnd);
    int ret = OnImagePaint(hdc);
    //BitBlt (m_hdcMem, 0, 0, m_w, m_h, hdc, 0, 0, 0);

    EndPaint(hwnd,hdc);
}
int  CImage::internal_proc(HWND hwnd, int message, WPARAM w, LPARAM l)
{
 //fprintf(stderr,"message=%d\n",message);
    if( (message == MSG_PAINT))
    {

        return internal_on_paint(hwnd);
        //return old_wnd_proc(hwnd,message,w,l);
    }else if(message == MSG_ERASEBKGND){
        return OnEraseBk(m_hdcMem);
    }else{
        return old_wnd_proc(hwnd,message,w,l);
    }

}
bool CImage::Attach(CCommCtrl* ctrl)
{
    m_ctrl  = ctrl;

    m_hwnd =  GetDlgItem(m_parent->m_hWnd,m_ctrl->GetId());

    if(m_hwnd != HWND_INVALID)
    {

        HDC hdc  = GetClientDC(m_hwnd);
        m_hdcMem = CreateCompatibleDC(hdc);
        ReleaseDC(hdc);

        SetWindowAdditionalData2(m_hwnd,(DWORD)this);
        old_wnd_proc = SetWindowCallbackProc(m_hwnd,m_ctrl_proc);

    }
    else{
        fprintf(stderr,"can't attch hwnd\n");
    }

    return (m_hwnd!=HWND_INVALID);
}
CImage::~CImage()
{

}
