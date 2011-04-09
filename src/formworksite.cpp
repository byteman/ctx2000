#include "formworksite.h"
#include "tajidbmgr.h"
#include "comdata.h"
static WNDPROC old_static_proc;
static HWND    old_hwnd;
static CFormWorksite  *myptr = NULL;

void   CFormWorksite::calc_map_point(double x1, double y1,double r1,double s1,double &x2, double &y2,double &r2,double &s2)
{
    x2 =  (x1-m_local_x)*m_zoom + m_center_x;
    y2 =  (m_local_y-y1)*m_zoom + m_center_y;
    r2 = r1*m_zoom;
    s2 = s1*m_zoom;
}
void   CFormWorksite::init_map_point(int local_x, int local_y,int org_x, int org_y, int long_arm_len)
{
    m_width_factor  = (double)((0.7*m_width) /(2*long_arm_len));
    m_height_factor = (double)((0.7*m_height)/(2*long_arm_len));
    if( m_width_factor > m_height_factor )
    {
        m_zoom =    m_width_factor;
    }else{
        m_zoom =    m_height_factor;
    }
    m_local_x = local_x;
    m_local_y = local_y;
}
CFormWorksite::CFormWorksite(CStatic* area,int w,int h):
    m_area(area),
    m_width(w),m_height(h)
{
    size_t i = 0;
    double x,y,r,s;
    m_center_x = w/2;
    m_center_y = h/2;

    init_map_point(g_TC[g_local_id].x,g_TC[g_local_id].y,m_center_x,m_center_y,g_TC[g_local_id].LongArmLength);

    for(i = 0; i < g_conflict_tj_list.size(); i++)
    {
        int idx=g_conflict_tj_list.at(i);

        calc_map_point(g_TC[idx].x,g_TC[idx].y,g_TC[idx].LongArmLength,g_TC[idx].ShortArmLenght,x,y,r,s);

        m_tajis[i] = new CTaji(x,y,r,s,idx);
    }
    calc_map_point(g_TC[g_local_id].x,g_TC[g_local_id].y,g_TC[g_local_id].LongArmLength,g_TC[g_local_id].ShortArmLenght,x,y,r,s);
    m_tajis[i] = new CTaji(x,y,r,s,g_local_id);
    m_tj_num   = g_conflict_tj_list.size()+1;
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
