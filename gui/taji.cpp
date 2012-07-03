#include "taji.h"
#include <math.h>
#include "CppSQLite3.h"
#include <Poco/SingletonHolder.h>
#include <Poco/Format.h>
#include "tajidbmgr.h"
#include "comdata.h"

#define w    12
#define h    12

CTaji::CTaji(int x, int y, int r,int s, int id,double zoom):
    x_pt(x),y_pt(y),m_r(r),m_zoom(zoom),m_id(id)
{
    m_tj_num = Poco::format("%d",id);
    m_short_arm = s;
    SetRect(&m_rt,x_pt-m_r,y_pt-m_r,x_pt+m_r,y_pt+m_r);
    if(id==g_local_id)
    {
        m_is_local = true;
    }else{
        m_is_local = false;
    }
    old_angle    = 1000;
    old_position = 1000;
    m_long_arm_len = m_r;
    //fprintf(stderr,"taji rect x=%d y=%d r=%d b=%d\n",m_rt.left,m_rt.top,m_rt.right,m_rt.bottom);

}
bool CTaji::Update(HWND hwnd)
{
    InvalidateRect(hwnd,&m_rt,TRUE);
}

bool CTaji::Draw(HDC hdc)
{
    //return Draw(hdc,m_tj_num,g_angle,g_car_dist);
    //fprintf(stderr,"draw %d angle=%0.2f,pos=%0.2f\n",m_id,g_TC[m_id].Angle,g_TC[m_id].Position);
/*
    if(( fabs(g_TC[m_id].Angle-old_angle) < 0.005) && (fabs(g_TC[m_id].Position-old_position) < 0.005))
    {
        if(!is_update)
        {
            SetPenWidth(hdc, 2);
            SetPenColor(hdc,PIXEL_green);
            int w2 = 2*m_r;
            ArcEx(hdc,x_pt-m_r,y_pt-m_r,w2,w2,0 ,360*64);
            return false;
        }
    }
*/
    //is_update = false;

    return Draw(hdc,m_tj_num,g_TC[m_id].Angle,g_TC[m_id].Position);


}
bool CTaji::ErasePrev(HDC hdc)
{
    static bool first= true;
    if(first)
    {
        first=false;
        return false;
    }
    SetPenColor(hdc,PIXEL_lightwhite);
    SetPenWidth(hdc, 8);
    LineEx(hdc,x_pt,y_pt,old_pt_x1,old_pt_y1);

    LineEx(hdc,x_pt,y_pt,old_pt_x2,old_pt_y2);


    SetBrushColor(hdc,PIXEL_lightwhite);
    FillBox(hdc,old_pt_x3,old_pt_y3,10,10);

    return true;
}
/*
在画布上绘制塔机的实时状态
hdc:画布句柄
tjnum:塔机的编号
angle:塔机的回转角度 角度*2*pi/360;
car_dist:小车的幅度
*/
bool CTaji::Draw(HDC hdc,std::string tjnum,double angle,double car_dist)
{
     double x ,y;
     int width = 0;
     SetPenWidth(hdc, 2);
     SetPenColor(hdc,PIXEL_green);
#if 1
     if(m_id != g_local_id && !g_TC[m_id].Valide)
     {
         RECT rect;

         SetBrushColor(hdc,PIXEL_lightgray);
         SetRect(&rect,x_pt-2*w,y_pt+2*h,x_pt+2*w,y_pt-2*h);
         FillBox(hdc,x_pt-w,y_pt-h,w+h,w+h);
         SetTextColor (hdc,PIXEL_red);
         SetBkMode (hdc,BM_TRANSPARENT);
         SelectFont(hdc,GetSystemFont(SYSLOGFONT_DEFAULT));
         //fprintf(stderr,"font=%s\n",font->sbc_devfont->name);

         DrawText(hdc,m_tj_num.c_str(),m_tj_num.length (),&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
         return true;
     }
#endif
     if(g_TC[m_id].Dyna)
     {
        //fprintf(stderr,"Postion=%0.2f\n",g_TC[m_id].Position*m_zoom);
        m_r  = (g_TC[m_id].LongArmLength*cos(g_TC[m_id].Dang*3.1415/180)-g_TC[m_id].a0)*m_zoom;
        //m_r = g_TC[m_id].Position*m_zoom; //动臂式塔机的半径，就等于小车的幅度
     }else{
        m_r = m_long_arm_len;
     }
     width = 2*m_r;
     ArcEx(hdc,x_pt-m_r,y_pt-m_r,width,width,0 ,360*64);


 //长臂
     y = m_r * sin(angle);
     x = m_r * cos(angle);
     //fprintf(stderr,"x=%0.2f,y=%0.2f\n",x,y);
     SetPenColor(hdc,PIXEL_blue);
     SetPenWidth(hdc, 8);
     LineEx(hdc,x_pt,y_pt,x_pt + x,y_pt-y);
     //fprintf(stderr,"angle=%0.2f,x=%0.2f,y=%0.2f\n",angle,sin(angle),cos(angle));

//短臂
     SetPenColor(hdc,PIXEL_darkyellow);
     y = m_short_arm * sin(angle);
     x = m_short_arm * cos(angle);
     LineEx(hdc,x_pt,y_pt,x_pt - x,y_pt+y);
     /*
     width = 2*m_short_arm;
     SetPenType(hdc,PT_ON_OFF_DASH);
     SetPenWidth(hdc, 1);
     SetPenColor(hdc,PIXEL_magenta);
     ArcEx(hdc,x_pt-m_short_arm,y_pt-m_short_arm,width,width,0 ,360*64);
     SetPenType(hdc,PT_SOLID);
     SetPenColor(hdc,PIXEL_lightgray);
     */
//原点的塔机编号和边框
     RECT rect;
     SetBrushColor(hdc,PIXEL_lightgray);
     SetRect(&rect,x_pt-2*w,y_pt+2*h,x_pt+2*w,y_pt-2*h);
     FillBox(hdc,x_pt-w,y_pt-h,w+h,w+h);
     SetTextColor (hdc,PIXEL_red);
     SetBkMode (hdc,BM_TRANSPARENT);
     SelectFont(hdc,GetSystemFont(SYSLOGFONT_DEFAULT));
     DrawText(hdc,m_tj_num.c_str(),m_tj_num.length (),&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

//小车
     SetBrushColor(hdc,PIXEL_red);
     if(g_TC[m_id].Dyna)
     {
         x = x_pt + m_r * cos(angle);
         y = y_pt - m_r * sin(angle)-h/2;
     }else{
         x = x_pt + car_dist * cos(angle)*m_zoom;
         y = y_pt - car_dist * sin(angle)*m_zoom-h/2;
     }

     FillBox(hdc,x-5,y,10,10);
     return true;
}

CTaijiMgr& CTaijiMgr::Get()
{
   static Poco::SingletonHolder<CTaijiMgr> sh;
   return *sh.get();
}
CTaijiMgr::CTaijiMgr()
{

}

int CTaijiMgr::Init(int num)
{    
}
