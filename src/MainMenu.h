// MainMenu.h: interface for the CMainMenu class.  // ////////////////////////////////////////////////////////////////////// 
#if !defined(AFX_MAINMENU_H__D30D85AD_3072_4144_A340_2273333C1696__INCLUDED_)
#define AFX_MAINMENU_H__D30D85AD_3072_4144_A340_2273333C1696__INCLUDED_

#include "SkinForm.h"
#include "dt_core.h"
#include "dirstatus.h"
#include "taji.h"
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

private:
        void CreateStatusArea(HDC hdc,RECT rt);
        void DrawDevSerial(HDC hdc, RECT rt,std::string devserail);
        CSkinButton* _skinBtns[MM_SKIN_BTN_NUM];
	int m_msg_delay;
        CFont* Font24;
        CStatusIcon* statusIcon[20];
        RECT m_status_rect;
        RECT m_dev_serail_rect;
        CDirStatusMgr* m_dir_mgr;

        CEdit* edt_long_arm_len;
        CEdit* edt_short_arm_len;
        CEdit* edt_height;
        CEdit* edt_up_angle,*edt_weight,*edt_fengsu,*edt_dg_height;
        CStatic* lbl_angle;
        CStatic* lbl_dist;
        CStatic* m_areas[4];
        CStatic* lbl_rights[3];
        CFormWorksite* m_worksite;
        CIcon m_angle;
        CIcon m_dist;

        bool m_show_angle;
};

#endif // !defined(AFX_MAINMENU_H__D30D85AD_3072_4144_A340_2273333C1696__INCLUDED_)
