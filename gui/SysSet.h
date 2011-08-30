// MainMenu.h: interface for the CMainMenu class.  // ////////////////////////////////////////////////////////////////////// 
#if !defined(_SYSSET_H)
#define _SYSSET_H

#include "SkinForm.h"
#include "dt_core.h"
#include "taji.h"
#define SYS_SKIN_BTN_NUM  7
class CSysSet : public CSkinForm
{
public:
    CSysSet();
    virtual ~CSysSet();
    virtual void OnCreate();
    virtual void OnClose();
    virtual void OnShow();
    virtual void OnPaint(HWND hWnd);
    virtual void OnButtonClick(skin_item_t* item);

private:
    CSkinButton* _skinBtns[SYS_SKIN_BTN_NUM];
    CSkinChkButton* _chkBtns[2];
    int     m_cur_mode;
};

#endif // !defined(AFX_MAINMENU_H__D30D85AD_3072_4144_A340_2273333C1696__INCLUDED_)
