#include "formworksite.h"
#include "tajidbmgr.h"
#include "comdata.h"


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

        m_tajis[i] = new CTaji(x,y,r,s,idx,m_zoom);
    }
    calc_map_point(g_TC[g_local_id].x,g_TC[g_local_id].y,g_TC[g_local_id].LongArmLength,g_TC[g_local_id].ShortArmLenght,x,y,r,s);
    m_tajis[i]      = new CTaji(x,y,r,s,g_local_id,m_zoom);
    m_tj_num        = g_conflict_tj_list.size()+1;
    old_hwnd        = HWND_INVALID;
    old_static_proc = NULL;
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

void CFormWorksite::draw_zhangai(HDC hdc)
{
    static POINT area[6];

    gal_pixel old = SetBrushColor(hdc,PIXEL_red);
 /*
    int oft_x=-50,oft_y=100;
    area[0].x = 100+oft_x;
    area[0].y = 100+oft_y;
    area[1].x = 200+oft_x;
    area[1].y = 100+oft_y;
    area[2].x = 200+oft_x;
    area[2].y = 200+oft_y;
    area[3].x = 100+oft_x;
    area[3].y = 200+oft_y;
    gal_pixel old = SetBrushColor(hdc,PIXEL_red);
*/

        for(size_t i=0; i <wbNum;i++)
        {
            //fprintf(stderr,"i=%d num=%d h=%0.2f\n",i,wba[i].VertexNum,wba[i].h);
            if( (wba[i].VertexNum>0) && (wba[i].VertexNum<6))
            {
                //fprintf(stderr,"l_x=%d,zom=%0.2f,c_x=%d\n",m_local_x,m_zoom,m_center_x);
                for(int j =0; j < wba[i].VertexNum;j++)
                {

                    double x = wba[i].Pointxy[j][0];
                    double y = wba[i].Pointxy[j][1];
                    area[j].x =  (x-m_local_x)*m_zoom + m_center_x;
                    area[j].y =  (m_local_y-y)*m_zoom + m_center_y;
                    if(area[j].x < 0)area[j].x=0;
                    if(area[j].y < 0)area[j].y=0;
                    //fprintf(stderr,"j=%d,x=%d,y=%d\n",j,area[j].x,area[j].y);
                }


            }
            FillPolygon(hdc,area,wba[i].VertexNum);
        }



    SetBrushColor(hdc,old);

}
int  CFormWorksite::worksite_proc(HWND hwnd, int message, WPARAM w, LPARAM l)
{
    static bool draw_flag=false;
    HDC hdc = BeginPaint(hwnd);

    SetPenColor(hdc,PIXEL_lightgray);
    Rectangle(hdc,0,0,m_width-1,m_height);

    for( size_t i = 0 ; i < g_conflict_tj_list.size()+1; i++)
    {
        m_tajis[i]->Draw(hdc);
    }
    if(!draw_flag)
    {
        draw_zhangai(hdc);
        draw_flag=true;
    }

    EndPaint(hwnd,hdc);
    return 1;
}
int static_proc(HWND hwnd, int message, WPARAM w, LPARAM l)
{
    CFormWorksite* myptr = (CFormWorksite*)GetWindowAdditionalData2(hwnd);
    if(myptr)
    {
        if(message == MSG_PAINT)
            return myptr->worksite_proc(hwnd,message,w,l);
        else{
            return myptr->old_static_proc(hwnd,message,w,l);
        }
    }

}
void CFormWorksite::init(HWND parent)
{
    old_hwnd = GetDlgItem(parent,m_area->GetId());
    if(old_hwnd != HWND_INVALID)
    {
        HDC hdc = GetClientDC(old_hwnd);
        m_hdcMem = CreateCompatibleDC(hdc);
        ReleaseDC(hdc);
        //SetWindowAdditionalData2(old_hwnd,(DWORD)this);
        //old_static_proc = SetWindowCallbackProc(old_hwnd,static_proc);
    }
    else{
        fprintf(stderr,"can't get hwnd\n");
    }
}
bool CFormWorksite::addTaji(QtzParam* tzPar)
{

}
void CFormWorksite::updateAll()
{
    for(int i = 0; i < m_tj_num; i++)
        m_tajis[i]->is_update=true;
}
//更新某个编号的塔机区域，如果0的话，就是更新所有塔机状态
void CFormWorksite::update(int taji)
{
    //UpdateWindow(old_hwnd,FALSE);

    static bool draw_flag=false;
    HDC hdc = GetClientDC(old_hwnd);

    SetBrushColor(m_hdcMem,PIXEL_lightwhite);
    FillBox(m_hdcMem,0,0,m_width,m_height);
    SetPenColor(hdc,PIXEL_lightgray);
    Rectangle(m_hdcMem,0,0,m_width-1,m_height);

    for( size_t i = 0 ; i < g_conflict_tj_list.size()+1; i++)
    {
        m_tajis[i]->Draw(m_hdcMem);
    }
    if(!draw_flag)
    {
        draw_zhangai(m_hdcMem);
        //draw_flag=true;
    }
    BitBlt (m_hdcMem, 0, 0, m_width, m_height, hdc, 0, 0, 0);
    ReleaseDC(hdc);
    return ;
}
