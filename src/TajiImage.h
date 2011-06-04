#ifndef TAJIIMAGE_H
#define TAJIIMAGE_H

#include "Image.h"
typedef struct tag_TowerInfo{
    int id;
    int x,y,r,h;
}TTowerInfo;
class CTajiImage : public CImage
{
public:
    CTajiImage(COMM_CTRL_DESC* desc,CSkinForm* parent);
    int AddNewTaji(TTowerInfo info);
    void UpdateInfo(double angle, double position);
    virtual int OnImagePaint(HDC hdc);
    virtual int OnEraseBk(HDC hdc);
private:
    //std::vector<TTowerInfo> m_TowerList;
    TTowerInfo m_info;
    int m_r;
    int x_pt,y_pt;
    double angle;
    double m_short_arm,m_long_arm;
    double car_dist;
};

#endif // TAJIIMAGE_H
