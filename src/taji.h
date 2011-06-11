#ifndef TAJI_H
#define TAJI_H
#include "collalgo.h"
#include "dt_core.h"
#include <string>
class CTaji
{
public:
    CTaji(int x, int y, int r, int s,int id,double zoom);
    bool Draw(HDC hdc);
    bool ErasePrev(HDC hdc);
    bool Draw(HDC hdc,std::string tjnum,double angle,double car_dist);
    bool Update(HWND hwnd);
    bool is_update;
private:
    double old_angle ,old_position;
    QtzParam* m_tzPar;
    double x_pt,y_pt,m_r,m_long_arm_len,m_short_arm;
    double old_pt_x1,old_pt_y1,old_pt_x2,old_pt_y2,old_pt_x3,old_pt_y3;
    int  m_id;
    std::string m_tj_num;
    RECT m_rt;
    bool m_is_local;
    double m_zoom;

};
class CTaijiMgr{
public:
    CTaijiMgr();
    static CTaijiMgr& Get();
    int Init(int num);
    bool addParam();
    int GetTotalNum()
    {
        return m_total_num;
    }
private:
    int  m_total_num;
};
#endif // TAJI_H
