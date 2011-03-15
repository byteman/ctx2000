#include "taji.h"
#include <math.h>
#include "CppSQLite3.h"
#include <Poco/SingletonHolder.h>
#include <Poco/Format.h>
#include "tajidbmgr.h"

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
CTaji::CTaji(int x, int y, int r):
    x_pt(x),y_pt(y),m_r(r)
{
    SetRect(&m_rt,x_pt-m_r,y_pt-m_r,x_pt+m_r,y_pt+m_r);
}
bool CTaji::Update(HWND hwnd)
{
    InvalidateRect(hwnd,&m_rt,TRUE);
}
bool CTaji::Draw(HDC hdc)
{
    return Draw(hdc,m_tj_num,m_tzPar->long_arm_angle,m_tzPar->carrier_pos);
}
bool CTaji::Draw(HDC hdc,std::string tjnum,double angle,double car_dist)
{
     SetPenWidth(hdc, 5);
     SetPenColor(hdc,PIXEL_green);
     int w2 = 2*m_r;
     ArcEx(hdc,x_pt-m_r,y_pt-m_r,w2,w2,0 ,360*64);

 //长臂

     double x = m_r * sin(angle);
     double y = m_r * cos(angle);
     SetPenColor(hdc,PIXEL_blue);
     LineEx(hdc,x_pt,y_pt,x_pt + x,y_pt-y);
     //fprintf(stderr,"angle=%0.2f,x=%0.2f,y=%0.2f\n",angle,sin(angle),cos(angle));
 //短臂
     SetPenColor(hdc,PIXEL_darkyellow);
     x = m_short_arm * sin(angle);
     y = m_short_arm * cos(angle);

     LineEx(hdc,x_pt,y_pt,x_pt - x,y_pt+y);
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
     SetBrushColor(hdc,PIXEL_red);
     x = x_pt + car_dist * sin(angle);
     y = y_pt - car_dist * cos(angle);

     FillBox(hdc,x,y,10,10);
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
