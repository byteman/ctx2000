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
    bool Draw(HDC hdc,std::string tjnum,double angle,double car_dist);
    bool Update(HWND hwnd);
private:
    QtzParam* m_tzPar;
    double x_pt,y_pt,m_r,m_short_arm;
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
