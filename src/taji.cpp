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
    old_angle = 1000;
    old_position = 1000;
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
�ڻ����ϻ���������ʵʱ״̬
hdc:�������
tjnum:�����ı��
angle:�����Ļ�ת�Ƕ� �Ƕ�*2*pi/360;
car_dist:С���ķ���
*/
bool CTaji::Draw(HDC hdc,std::string tjnum,double angle,double car_dist)
{
     double x ,y;

     //ErasePrev(hdc);
     SetPenWidth(hdc, 2);
     SetPenColor(hdc,PIXEL_green);
     int w2 = 2*m_r;
     ArcEx(hdc,x_pt-m_r,y_pt-m_r,w2,w2,0 ,360*64);
     //fprintf(stderr,"angle=%0.2f\n",angle);
     //angle=angle*3.14/180;

 //����

     //if(m_is_local)
     {
         y = m_r * sin(angle);
         x = m_r * cos(angle);
         //fprintf(stderr,"x=%0.2f,y=%0.2f\n",x,y);
         SetPenColor(hdc,PIXEL_blue);
         SetPenWidth(hdc, 8);
         LineEx(hdc,x_pt,y_pt,x_pt + x,y_pt-y);
         old_pt_x1 = x_pt + x;
         old_pt_y1 = y_pt - y;
         //fprintf(stderr,"angle=%0.2f,x=%0.2f,y=%0.2f\n",angle,sin(angle),cos(angle));
     //�̱�
         SetPenColor(hdc,PIXEL_darkyellow);
         y = m_short_arm * sin(angle);
         x = m_short_arm * cos(angle);

         LineEx(hdc,x_pt,y_pt,x_pt - x,y_pt+y);
         old_pt_x2 = x_pt - x;
         old_pt_y2 = y_pt + y;
     }
//ԭ��ı��
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

//С��
     //if(m_is_local)
     {
         SetBrushColor(hdc,PIXEL_red);
         x = x_pt + car_dist * cos(angle)*m_zoom;
         y = y_pt - car_dist * sin(angle)*m_zoom-h/2;
         old_pt_x3 = x;
         old_pt_y3 = y;
         FillBox(hdc,x,y,10,10);
     }
      old_angle    = angle;
      old_position = car_dist;
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
