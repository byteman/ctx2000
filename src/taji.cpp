#include "taji.h"
#include <math.h>
#include "CppSQLite3.h"
#include <Poco/SingletonHolder.h>
#include <Poco/Format.h>
#include "tajidbmgr.h"
#include "comdata.h"
#define w    10
#define h    8


CTaji::CTaji(QtzParam* tzPar,int zoom):
    m_tzPar(tzPar)
{
    m_tzPar->height;
    x_pt = (tzPar->pos.x)/zoom;
    y_pt = (tzPar->pos.y)/zoom;
    m_r  = (tzPar->long_arm_len) / zoom;
    m_short_arm = (tzPar->short_arm_len) / zoom;
    SetRect(&m_rt,x_pt-m_r,y_pt-m_r,x_pt+m_r,y_pt+m_r);
    fprintf(stderr,"taji rect x=%d y=%d r=%d b=%d\n",m_rt.left,m_rt.top,m_rt.right,m_rt.bottom);
    m_tj_num = Poco::format("%u",tzPar->number);
}
CTaji::CTaji(int x, int y, int r,int s, int id):
    x_pt(x),y_pt(y),m_r(r)
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
    return Draw(hdc,m_tj_num,g_angle,g_car_dist);
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
     fprintf(stderr,"angle=%0.2f\n",angle);
     angle=angle*3.14/180;

 //长臂

     if(m_is_local)
     {
         y = m_r * sin(angle);
         x = m_r * cos(angle);
         fprintf(stderr,"x=%0.2f,y=%0.2f\n",x,y);
         SetPenColor(hdc,PIXEL_blue);
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
     SetBrushColor(hdc,PIXEL_lightwhite);
     SetRect(&rect,x_pt-w,y_pt+h,x_pt+w,y_pt-h);
     FillBox(hdc,x_pt-w,y_pt-h,w+h,w+h);
     SetTextColor (hdc,PIXEL_red);
     SetBkMode (hdc,BM_TRANSPARENT);
     //SetTextColor (hdc,PIXEL_red);
     SelectFont(hdc,GetSystemFont(SYSLOGFONT_FIXED));
     DrawText(hdc,m_tj_num.c_str(),m_tj_num.length (),&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    //Circle(hdc,m_tzPar->pos.x,m_tzPar->pos.y);
//小车
     if(m_is_local)
     {
         SetBrushColor(hdc,PIXEL_red);
         x = x_pt + car_dist * sin(angle);
         y = y_pt - car_dist * cos(angle);

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
