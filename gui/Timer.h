#ifndef TIMER_CLASS_H
#define TIMER_CLASS_H

#include "SkinForm.h"
#include "dt_core.h"
#define TIMER_EDIT_NUM 3
#define TIMER_BTN_NUM  3
class CTimer : public CSkinForm
{
public:

    CTimer();
    virtual ~CTimer();
    virtual void   OnCreate();
    virtual void   OnTimer(int ID);
    virtual void OnCommCtrlNotify(HWND hwnd, int id, int nc);
    virtual void   OnPaint(HWND hWnd);
    virtual void   OnClose();
    virtual void OnShow();
    virtual void   OnButtonClick(skin_item_t* item);
    virtual void OnCommand(HWND hWnd, int message, WPARAM wParam,LPARAM lParam);

private:
    HWND m_hCalendar;
    MCCOLORINFO color;
    int  m_year,m_mon,m_day,m_hour,m_min,m_sec;
    BOOL m_timeChanged;
    BOOL m_dateChanged;
    RECT m_rect;

    CEdit* _edit[TIMER_EDIT_NUM];
    CSkinButton* _skinBtns[TIMER_BTN_NUM];
    CIconGroup _lables;

};

#endif //TIMER_CLASS_H

