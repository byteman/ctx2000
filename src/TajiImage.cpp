#include "TajiImage.h"
#include <math.h>
#define w    12
#define h    12

CTajiImage::CTajiImage(COMM_CTRL_DESC* desc,CSkinForm* parent)
    :CImage(desc,parent)
{

    m_r = 100;
    x_pt = 100;
    y_pt = 100;
    angle = 3.14/180*3;
    m_short_arm=20;
    m_long_arm=100;
    car_dist = 20;
}
void CTajiImage::UpdateInfo(double angle, double position)
{
    car_dist       = position;
    this->angle    = angle;


    UpdateWindow(m_hwnd,FALSE);
}
int CTajiImage::OnEraseBk(HDC hdc)
{
    SetBrushColor(hdc,PIXEL_lightwhite);
    FillBox(hdc,0,0,m_w,m_h);
}
int CTajiImage::OnImagePaint(HDC hdc)
{
    //fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
    double x,y;
    SetBkColor(hdc,PIXEL_lightwhite);
    //SetBrushColor(hdc,PIXEL_lightwhite);
    //FillBox(hdc,0,0,m_w,m_h);
    //fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
    SetPenWidth(hdc, 2);
    //fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
    SetPenColor(hdc,PIXEL_green);
    //fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
    int w2 = 2*m_r;
    //fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
    ArcEx(hdc,x_pt-m_r,y_pt-m_r,w2,w2,0 ,360*64);

    //fprintf(stderr,"angle=%0.2f\n",angle);
    //angle=angle*3.14/180;
//fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
//长臂

    //if(m_is_local)
    {
        y = m_r * sin(angle);
        x = m_r * cos(angle);
        //fprintf(stderr,"x=%0.2f,y=%0.2f\n",x,y);
        SetPenColor(hdc,PIXEL_blue);
        SetPenWidth(hdc, 8);
        LineEx(hdc,x_pt,y_pt,x_pt + x,y_pt-y);
//fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
        //fprintf(stderr,"angle=%0.2f,x=%0.2f,y=%0.2f\n",angle,sin(angle),cos(angle));
    //短臂
        SetPenColor(hdc,PIXEL_darkyellow);
        y = m_short_arm * sin(angle);
        x = m_short_arm * cos(angle);

        LineEx(hdc,x_pt,y_pt,x_pt - x,y_pt+y);

    }
//fprintf(stderr,"%s %d\n",__FUNCTION__,__LINE__);
//原点的编号
    RECT rect;
    SetBrushColor(hdc,PIXEL_lightgray);
    SetRect(&rect,x_pt-2*w,y_pt+2*h,x_pt+2*w,y_pt-2*h);
    FillBox(hdc,x_pt-w,y_pt-h,w+h,w+h);
    //Rectangle(hdc,x_pt-w,y_pt-h,x_pt+w,y_pt+h);
    //SetTextColor (hdc,PIXEL_red);
    //SetBkMode (hdc,BM_TRANSPARENT);
    //SetTextColor (hdc,PIXEL_red);
    //SelectFont(hdc,GetSystemFont(SYSLOGFONT_DEFAULT));
    //DrawText(hdc,m_tj_num.c_str(),m_tj_num.length (),&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

//小车
    //if(m_is_local)
    {
        SetBrushColor(hdc,PIXEL_red);
        x = x_pt + car_dist * cos(angle);
        y = y_pt - car_dist * sin(angle)-h/2;

        FillBox(hdc,x,y,10,10);
    }
}
