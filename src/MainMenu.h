// MainMenu.h: interface for the CMainMenu class.  // ////////////////////////////////////////////////////////////////////// 
#if !defined(AFX_MAINMENU_H__D30D85AD_3072_4144_A340_2273333C1696__INCLUDED_)
#define AFX_MAINMENU_H__D30D85AD_3072_4144_A340_2273333C1696__INCLUDED_

#include "SkinForm.h"
#include "dt_core.h"
#include "dirstatus.h"
#include "taji.h"
#include "percent.h"
#include "FastStatic.h"
#define MM_SKIN_BTN_NUM  3
class CStatusIcon;
class CFormWorksite;
class CMainMenu : public CSkinForm  
{
public:
	CMainMenu();
	virtual ~CMainMenu();
	virtual void OnCreate();
	virtual void OnTimer(int ID);
        virtual void OnPaint(HWND hWnd);
        virtual void OnShow();
	virtual void OnButtonClick(skin_item_t* item);
        virtual void OnLButtonUp(int x, int y);
        virtual void OnUserMsg(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

private:
        void UpdateUIArea();
        void CreateStatusArea(HDC hdc,RECT rt);
        void CreateInfoArea(HDC hdc);
        void EmulateSensor();
        void DrawTCType(HDC hdc, RECT rt,std::string tctype);
        void DrawDevSerial(HDC hdc, RECT rt,std::string devserail);
        CSkinButton* _skinBtns[5];
	int m_msg_delay;
        CFont* Font24;
        CFont* Font40;
        CStatusIcon* statusIcon[20];
        RECT m_status_rect;
        RECT m_dev_serail_rect;
        RECT m_tc_type_rect;
        RECT m_fall_rect,m_quit_rect;
        CDirStatusMgr* m_dir_mgr;

        CStatic* edt_angle;
        CStatic* edt_dist;
        CStatic* edt_weight;
        CStatic* edt_fall;
        CStatic* edt_tower_height;
        CStatic* edt_long_arm_len;
        CStatic* edt_short_arm_len;
        CStatic* edt_height;
        CStatic* edt_up_angle,*edt_max_weight,*edt_fengsu,*edt_dg_height;

        CFastStatic* fast_angle;
        CFastStatic* fast_dist;
        CFastStatic* fast_weight;
        CFastStatic* fast_beilv;
        CFastStatic* fast_tower_height;
        CFastStatic* fast_long_arm_len;
        CFastStatic* fast_short_arm_len;
        CFastStatic* fast_height;
        CFastStatic* fast_up_angle,*fast_max_weight,*fast_fengsu,*fast_dg_height;

        CStatic* lbl_angle;
        CStatic* lbl_dist;
        CStatic* lbl_beilv;
        CStatic* lbl_max_weight;
        CStatic* m_areas[4];
        CStatic* lbl_rights[3];
        CStatic* m_liju;
        CFormWorksite* m_worksite;

        CIconGroup m_lables;
        CPercent m_per;
        HDC m_hdc;
        bool m_quit;
        bool m_show_up_angle,m_show_speed,m_show_dg_height,m_show_max_weight;
};

#endif // !defined(AFX_MAINMENU_H__D30D85AD_3072_4144_A340_2273333C1696__INCLUDED_)
