#ifndef TAJI_H
#define TAJI_H
#include "collalgo.h"
#include "dt_core.h"
#include <string>
class CTaji
{
public:
    CTaji(int x, int y, int r);
    //param: 全局的塔机参数字
    CTaji(QtzParam* tzPar,int zoom=1);
    bool Draw(HDC hdc);
    bool Draw(HDC hdc,std::string tjnum,double angle,double car_dist);
    bool Update(HWND hwnd);
private:
    QtzParam* m_tzPar;
    double x_pt,y_pt,m_r,m_short_arm;
    std::string m_tj_num;
    RECT m_rt;

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
