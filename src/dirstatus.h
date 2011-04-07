#ifndef DIRSTATUS_H
#define DIRSTATUS_H
#include <string>
#include "ctx2000.h"
#include "dt_core.h"
class CDirStatus
{
public:
    CDirStatus(std::string type);
    void SetRect(RECT rt)
    {
        m_pos = rt;
    }
    bool SetStatus(HDC hdc,EDevStatus status);
public:
    CIcon m_icon;
    RECT  m_pos;
    std::string   m_type;
};

class CDirStatusMgr
{
public:
    CDirStatusMgr(int left, int top);
    bool Show(HDC hdc, std::string type, EDevStatus status);
    bool Show(HDC hdc, EDevStatus up,EDevStatus down,EDevStatus left,EDevStatus right);
private:
    RECT m_upRect,m_downRect,m_leftRect,m_rightRect,m_centerRect;
    CDirStatus *m_dirs[4];
    RECT m_areaRect;
};
#endif // DIRSTATUS_H
