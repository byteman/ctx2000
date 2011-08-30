#ifndef DT_PERCENT_H
#define DT_PERCENT_H
#include "dt_core.h"

typedef struct tag_ColorPercent{
    double      value;
    gal_pixel   color;
}TColorPercent;
typedef std::vector<TColorPercent> TColorSet;
class DT_Percent
{
public:
    DT_Percent(COMM_CTRL_DESC* desc,CSkinForm* parent,int direction=0);
    ~DT_Percent();
    bool Create();
    void Show(double value);
    void SetColorPercent(TColorSet &rst);
private:
    bool HasInit();
    void PaintOnDC(HDC hdc,double value);
    CStatic* m_ctrl;
    CSkinForm* m_parent;
    int       m_w,m_h;
    gal_pixel m_bgcolor,m_fgcolor;
    double    m_percent;
    int       m_direction;
    HWND      m_hwnd;
    HDC       m_hdc,m_MemDC;
    RECT      m_rect;
    TColorSet m_colors;
    std::string m_text;

    bool flag;

};

#endif // PERCENT_H
