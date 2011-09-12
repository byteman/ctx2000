// MainMenu.h: interface for the CMainMenu class.  // ////////////////////////////////////////////////////////////////////// 
#if !defined(AFX_MAINMENU_H__D30D85AD_3072_4144_A340_2273333C1696__INCLUDED_)
#define AFX_MAINMENU_H__D30D85AD_3072_4144_A340_2273333C1696__INCLUDED_

#include "SkinForm.h"
#include "dt_core.h"
#include "dirstatus.h"
#include "taji.h"
#include "dt_percent.h"
#include "FastStatic.h"
#include "percent.h"
#include "filter.h"
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
        void LoadOptionItem();
        void UpdateUIArea(bool first=true);
        void CreateStatusArea(HDC hdc,RECT rt);
        void CreateInfoArea(HDC hdc);
        void EmulateSensor();
        void DrawTCType(HDC hdc, RECT rt,std::string tctype);
        void DrawMsg(int id,std::string msg);
        void DrawDevSerial(HDC hdc, RECT rt,std::string devserail);
        void UpdateCollideStatus(int type, bool flag);
        void UpdateCollideStatus();
        __inline__ void UpdateSignal();
        __inline__ void UpdateRealTimeParam(bool update=false);
        __inline__ void UpdateDevMode();
        CSkinButton* _skinBtns[5];
        int     m_msg_delay;
        CFont* Font16;
        CFont* Font24;
        CFont* Font32;
        CFont* Font40;
        CStatusIcon* statusIcon[20];
        RECT m_status_rect;
        RECT m_dev_serail_rect;
        RECT m_tc_type_rect;
        RECT m_fall_rect,m_quit_rect,m_msg_rect,m_signal_rect;
        CDirStatusMgr* m_dir_mgr;

        CFastStatic *fast_msg[2];

        CFastStatic* fast_angle;
        CFastStatic* fast_dist;
        CFastStatic* fast_weight;
        CFastStatic* fast_fall;
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
        CStatic* m_fall;
        CFormWorksite* m_worksite;


        CIconGroup m_lables;
        CIcon m_signal;
        CPercent m_per;
        EWorkMode m_mode;
        HDC m_hdc;
        bool m_quit;
        int  m_cur_signal_db;
        int  m_cur_option_pos;
        bool m_need_update;
};

#endif // !defined(AFX_MAINMENU_H__D30D85AD_3072_4144_A340_2273333C1696__INCLUDED_)
