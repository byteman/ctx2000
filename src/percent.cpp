#include "percent.h"
#include <stdio.h>
#include <math.h>
static WNDPROC old_static_proc;
static HWND    old_hwnd;
static CPercent  *myptr = NULL;
CPercent::CPercent()
{
    m_hwnd=HWND_INVALID;
    myptr    = this;
}

static int m_static_proc(HWND hwnd, int message, WPARAM w, LPARAM l)
{
    if( (message == MSG_PAINT) && (hwnd == old_hwnd))
    {
        if(myptr)
            return myptr->worksite_proc(hwnd,message,w,l);
    }else{
        return old_static_proc(hwnd,message,w,l);
    }

}
int  CPercent::worksite_proc(HWND hwnd, int message, WPARAM w, LPARAM l)
{
    HDC hdc = BeginPaint(hwnd);


    char buf[32] = {0,};
    int tmp = m_percent*100.0f+0.5;
    snprintf(buf,32,"%d%",tmp);

    //fprintf(stderr,"text=%s height=%d\n",buf,height);
    SetBkMode(hdc,BM_TRANSPARENT);
    SelectFont(hdc,GetSystemFont(SYSLOGFONT_FIXED));
    DrawText(hdc,buf,strlen(buf),&m_rect,DT_TOP|DT_LEFT);

    SetBrushColor(hdc,COLOR_lightwhite);
    FillBox(hdc,0,30,m_w,m_h-old_height-30);//绘制上面的空白区域
    if(tmp<90)
    {
        SetBrushColor(hdc,COLOR_green);
    }else if(tmp < 100){

         SetBrushColor(hdc,COLOR_yellow);
    }else if(tmp < 105){
        SetBrushColor(hdc,COLOR_red);
    }else{
        SetBrushColor(hdc,COLOR_red);
    }
//绘制百分比
    FillBox(hdc,0,m_h-old_height,m_w,old_height);

    EndPaint(hwnd,hdc);

return 0;
}
bool CPercent::Init(CSkinForm* parent,CStatic* ctrl,int w, int h)
{
    m_ctrl  = ctrl;
    m_w     = w;
    m_h     = h;
    m_parent= parent;

    m_hwnd =  GetDlgItem(m_parent->m_hWnd,m_ctrl->GetId());
    if(m_hwnd != HWND_INVALID){
       // m_hdc = GetClientDC(m_hwnd);
    }
    SetRect(&m_rect,0,0,w,h);

    if(m_hwnd != HWND_INVALID)
    {
        old_hwnd = m_hwnd;
        old_static_proc = SetWindowCallbackProc(old_hwnd,m_static_proc);
    }
    else{
        fprintf(stderr,"can't get hwnd\n");
    }

    return (m_hwnd!=HWND_INVALID);
}
CPercent::~CPercent()
{

}

void CPercent::Show(double percent)
{
    //static double tmp=0;
    //tmp+=0.01;
    //percent = tmp;
    //SetBrushColor(m_hdc,RGB2Pixel(HDC_SCREEN,255,255,255));
    //FillBox(m_hdc,0,0,m_w,m_h);
    m_percent = percent;
    if( fabs(percent) > 1.0)
        percent = 1.0;

    int height = (int)(percent*(m_h-30));
    if(height == old_height)
    {
        char buf[32] = {0,};
        int tmp = m_percent*100.0f+0.5;
        tmp = snprintf(buf,32,"%d",tmp);
        buf[tmp]='%';
        HDC hdc = GetClientDC(m_hwnd);
        //SetBkMode(hdc,BM_TRANSPARENT);
        SelectFont(hdc,GetSystemFont(SYSLOGFONT_FIXED));
        //SelectFont(hdc,GetSystemFont(SYSLOGFONT_CONTROL));
        DrawText(hdc,buf,strlen(buf),&m_rect,DT_TOP|DT_LEFT);
        ReleaseDC(hdc);
        flag = false;
        return;

    }else{
        flag = true;
    }
    old_height = height;
    UpdateWindow(m_hwnd,FALSE);

}
