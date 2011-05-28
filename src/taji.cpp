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
    fprintf(stderr,"taji rect x=%d y=%d r=%d b=%d\n",m_rt.left,m_rt.top,m_rt.right,m_rt.bottom);

}
bool CTaji::Update(HWND hwnd)
{
    InvalidateRect(hwnd,&m_rt,TRUE);
}
bool CTaji::Draw(HDC hdc)
{
    //return Draw(hdc,m_tj_num,g_angle,g_car_dist);
    //fprintf(stderr,"draw %d angle=%0.2f,pos=%0.2f\n",m_id,g_TC[m_id].Angle,g_TC[m_id].Position);
    return Draw(hdc,m_tj_num,g_TC[m_id].Angle,g_TC[m_id].Position);
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
     SetPenWidth(hdc, 2);
     SetPenColor(hdc,PIXEL_green);
     int w2 = 2*m_r;
     ArcEx(hdc,x_pt-m_r,y_pt-m_r,w2,w2,0 ,360*64);
     //fprintf(stderr,"angle=%0.2f\n",angle);
     //angle=angle*3.14/180;

 //长臂

     //if(m_is_local)
     {
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
     }
//原点的编号
     RECT rect;
     SetBrushColor(hdc,PIXEL_lightgray);
     SetRect(&rect,x_pt-2*w,y_pt+2*h,x_pt+2*w,y_pt-2*h);
     FillBox(hdc,x_pt-w,y_pt-h,w+h,w+h);
     //Rectangle(hdc,x_pt-w,y_pt-h,x_pt+w,y_pt+h);
     SetTextColor (hdc,PIXEL_red);
     SetBkMode (hdc,BM_TRANSPARENT);
     //SetTextColor (hdc,PIXEL_red);
     SelectFont(hdc,GetSystemFont(SYSLOGFONT_DEFAULT));
     DrawText(hdc,m_tj_num.c_str(),m_tj_num.length (),&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

//小车
     //if(m_is_local)
     {
         SetBrushColor(hdc,PIXEL_red);
         x = x_pt + car_dist * cos(angle)*m_zoom;
         y = y_pt - car_dist * sin(angle)*m_zoom-h/2;

         FillBox(hdc,x,y,10,10);
     }
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
