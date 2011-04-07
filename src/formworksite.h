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

private:
    CStatic* m_area;
    int m_width,m_height;
    CTaji* m_tajis[20];
    int    m_tj_num;
};

#endif // FORMWORKSITE_H
