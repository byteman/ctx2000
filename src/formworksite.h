#ifndef FORMWORKSITE_H
#define FORMWORKSITE_H
#include "taji.h"

class CFormWorksite
{
public:
    CFormWorksite(CStatic* area,int w,int h);
    ~CFormWorksite();
    bool addTaji(QtzParam* tzPar);
    void init(HWND parent);
    int  worksite_proc(HWND hwnd, int message, WPARAM w, LPARAM l);
    //更新某个编号的塔机区域，如果0的话，就是更新所有塔机状态
    void update(int taji=-1);
    void draw_zhangai(HDC hdc);
private:
    CStatic* m_area;
    int m_width,m_height;
    CTaji* m_tajis[20];
    int    m_tj_num;
    int    m_center_x,m_center_y,m_local_x,m_local_y;
    double    m_map_x,m_map_y,m_width_factor, m_height_factor,m_zoom;
    void   init_map_point(int local_x, int local_y,int org_x, int org_y ,int long_arm_len);
    void   calc_map_point(double x, double y,double r1,double s1,double &x2, double &y2,double &r2,double &s2);
};

#endif // FORMWORKSITE_H
