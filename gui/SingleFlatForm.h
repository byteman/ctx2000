#ifndef SINGLETAJI1_H
#define SINGLETAJI1_H

#include "SkinForm.h"
#include "dt_core.h"
#include "dirstatus.h"
#include "taji.h"
#include "percent.h"
#include "FastStatic.h"
#include "dt_percent.h"
#define MM_SKIN_BTN_NUM  3

class CSingleFlatForm : public CSkinForm
{
public:
    CSingleFlatForm();
    virtual ~CSingleFlatForm();
    virtual void OnCreate();
    virtual void OnTimer(int ID);
    virtual void OnPaint(HWND hWnd);
    virtual void OnShow();
    virtual void OnButtonClick(skin_item_t* item);
    virtual void OnLButtonUp(int x, int y);

private:
    void EmulateSensor();
    CSkinButton* _skinBtns[5];
    int m_msg_delay;
    CFont* Font40;
    CFont* Font24;
    gal_pixel color_black;
    RECT m_status_rect;
    RECT m_dev_serail_rect;
    RECT m_tc_type_rect;
    RECT m_liju_rect;
    RECT m_fall_rect;

    CStatic* m_liju;
    CStatic* edt_angle;
    CStatic* edt_dist;
    CStatic* edt_weight;
    CStatic* edt_fall;
    CStatic* edt_tower_height;
    CStatic* edt_long_arm_len;
    CStatic* edt_short_arm_len;
    CStatic* edt_height;
    CStatic* edt_up_angle,*edt_max_weight,*edt_fengsu,*edt_dg_height;
    CStatic* edt_percent;
    CStatic* edt_time;
    CFastStatic* fast_time;

    CFastStatic* fast_angle;
    CFastStatic* fast_dist;
    CFastStatic* fast_weight;
    CFastStatic* fast_beilv;
    CFastStatic* fast_tower_height;
    CFastStatic* fast_long_arm_len;
    CFastStatic* fast_short_arm_len;
    CFastStatic* fast_height;
    CFastStatic* fast_percent;
    CFastStatic* fast_up_angle,*fast_max_weight,*fast_fengsu,*fast_dg_height;

    DT_Percent *m_fall_fact,*m_dist_fact,*m_hi_fact;
    CIconGroup m_lables;
    CPercent m_per;
    HDC m_hdc;
};

#endif // !defined(AFX_MAINMENU_H__D30D85AD_3072_4144_A340_2273333C1696__INCLUDED_)
