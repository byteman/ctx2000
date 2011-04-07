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
    //����ĳ����ŵ������������0�Ļ������Ǹ�����������״̬
    void update(int taji=-1);

private:
    CStatic* m_area;
    int m_width,m_height;
    CTaji* m_tajis[20];
    int    m_tj_num;
};

#endif // FORMWORKSITE_H
