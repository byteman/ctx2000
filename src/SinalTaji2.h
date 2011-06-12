#ifndef SINGLETAJI2_H
#define SINGLETAJI2_H

#include "SkinForm.h"
#include "dt_core.h"
#include "dirstatus.h"
#include "taji.h"
#include "percent.h"
#include "FastStatic.h"
#include "dt_percent.h"
#define MM_SKIN_BTN_NUM  3

class CSingleTaji2 : public CSkinForm
{
public:
        CSingleTaji2();
        virtual ~CSingleTaji2();
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
        CFont* Font24;
        CFont* Font40;
        //CStatusIcon* statusIcon[20];

        RECT m_status_rect;
        RECT m_dev_serail_rect;
        RECT m_tc_type_rect;
        RECT m_liju_rect;


        CStatic* edt_angle;
        CStatic* edt_dist;
        CStatic* edt_weight;
        CStatic* edt_beilv;
        CStatic* edt_tower_height;
        CStatic* edt_long_arm_len;
        CStatic* edt_short_arm_len;
        CStatic* edt_height;
        CStatic* edt_up_angle,*edt_max_weight,*edt_fengsu,*edt_dg_height;
        CStatic* edt_percent;

        CFastStatic* fast_angle;
        CFastStatic* fast_dist;
        CFastStatic* fast_weight;
        CFastStatic* fast_beilv;
        CFastStatic* fast_tower_height;
        CFastStatic* fast_long_arm_len;
        CFastStatic* fast_short_arm_len;
        CFastStatic* fast_height,*fast_percent;
        CFastStatic* fast_up_angle,*fast_max_weight,*fast_fengsu,*fast_dg_height;

        CPercent m_per;
        DT_Percent* m_fall_fact;
        HDC m_hdc;
        gal_pixel color_black;
        bool m_show_up_angle,m_show_speed,m_show_dg_height,m_show_max_weight;
};

#endif // !defined(AFX_MAINMENU_H__D30D85AD_3072_4144_A340_2273333C1696__INCLUDED_)
