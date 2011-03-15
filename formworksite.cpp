#include "formworksite.h"
#include "tajidbmgr.h"
static WNDPROC old_static_proc;
static HWND    old_hwnd;
static CFormWorksite  *myptr = NULL;
CFormWorksite::CFormWorksite(CStatic* area,int w,int h):
    m_area(area),
    m_width(w),m_height(h)
{
    for(int i = 0; i < CTajiDbMgr::Get().get_tj_num(); i++)
    {
        m_tajis[i] = new CTaji(&g_qtzs[i]);
    }
    m_tj_num = CTajiDbMgr::Get().get_tj_num();
    myptr    = this;
}
CFormWorksite::~CFormWorksite()
{
    for( int i = 0 ; i < m_tj_num; i++)
    {
        if(m_tajis[i])
            delete m_tajis[i];
        m_tajis[i] = NULL;
    }
}
int  CFormWorksite::worksite_proc(HWND hwnd, int message, WPARAM w, LPARAM l)
{
    HDC hdc = BeginPaint(hwnd);

    Rectangle(hdc,0,0,m_width-1,m_height-1);

    for( int i = 0 ; i < m_tj_num; i++)
    {
        m_tajis[i]->Draw(hdc);
    }
    EndPaint(hwnd,hdc);
}
int static_proc(HWND hwnd, int message, WPARAM w, LPARAM l)
{
    if( (message == MSG_PAINT) && (hwnd == old_hwnd))
    {
        if(myptr)
            return myptr->worksite_proc(hwnd,message,w,l);
    }else{
        return old_static_proc(hwnd,message,w,l);
    }

}
void CFormWorksite::init(HWND parent)
{
    old_hwnd = GetDlgItem(parent,m_area->GetId());
    if(old_hwnd != HWND_INVALID)
        old_static_proc = SetWindowCallbackProc(old_hwnd,static_proc);
    else{
        fprintf(stderr,"can't get hwnd\n");
    }
}
bool CFormWorksite::addTaji(QtzParam* tzPar)
{

}
//更新某个编号的塔机区域，如果0的话，就是更新所有塔机状态
void CFormWorksite::update(int taji)
{
    if(taji == -1)
    {
        for( int i = 0 ; i < m_tj_num; i++)
        {
            //m_tajis[i]->Update(old_hwnd);
        }
    }else{
        //m_tajis[taji]->Update(old_hwnd);
    }
    //UpdateWindow(old_hwnd,TRUE);
    UpdateWindow(old_hwnd,FALSE);
    //if(m_area)
    //    m_area->SetText("");
}
