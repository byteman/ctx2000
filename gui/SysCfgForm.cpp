// SysCfgForm.cpp: implementation of the CSysCfgForm class.
//
//////////////////////////////////////////////////////////////////////
#include "SysCfgFormItem.h"
#include "SysCfgForm.h"
#include "BmpRes.h"

#include "Timer.h"
#include "IPset.h"
#include "SysLogForm.h"
#include "UISetForm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static const char *mmenu_bmps[] =
{
    PCOMM_BACKGROUND,
    SYSCFG_TIMER_BTN,
    SYSCFG_IPSET_BTN,
    QUERY_OPLOGMAN_BTN,
    PUI_PARM_BTN,
    "ctx2000/src_calib_btn.png",
    PCOMM_CLOSE_BTN,

};

static SKIN_BUTTON_DESC SkinBtns[] =
{
    SKIN_BUTTON_TIMER,
    SKIN_BUTTON_IPSET,
    SKIN_BUTTON_OPLOG,
    SKIN_BUTTON_UISET,
    SKIN_BUTTON_QUIT,
    SKIN_BUTTON_SOUND
};

static const char *icon_path[] =
{
    SYSCFG_SITE_ICON,
    SYSCFG_SITE_LABEL,
    SYSCFG_TIMER_LABEL,
    SYSCFG_IPSET_LABEL,
    QUERY_OPLOGMAN_LABEL,
    PUISET_LOGO,
    "ctx2000/src_calib_logo.png"
};

static POS BmpsPos[] =
{
    {
        SYSCFG_X,
        SYSCFG_Y
    },
    {
        SYSCFG_TEXT_X,
        SYSCFG_TEXT_Y
    },
    {
        TIMER_X,
        TIMER_Y
    },
    {
        IPSET_X,
        IPSET_Y
    },

    {
        EXTDEVSET_X,
        EXTDEVSET_Y
    },

    {
        IOSET_X,
        IOSET_Y
    },

    {
        SOUND_X,
        SOUND_Y
    }
};

CSysCfgForm::CSysCfgForm()
{
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }

    for (int i = 0; i < TABLESIZE(SkinBtns) ; i++)
        _skinBtns[i] = new CSkinButton(&SkinBtns[i], this);


    _menus.AddIcons(icon_path,ARRAY_SIZE(icon_path,char*));

    InitSkinHeader("CSysCfgForm");

}

CSysCfgForm::~CSysCfgForm()
{

}

void CSysCfgForm::OnShow()
{

}

void CSysCfgForm::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);

    for (int i = 0; i < TABLESIZE(icon_path); i++)
    {
        _menus.Show(hdc,BmpsPos[i].x,BmpsPos[i].y,i+1);
    }
    EndPaint(hWnd,hdc);
}
#include <stdlib.h>
void CSysCfgForm::calib_touch_screen()
{
    system("/usr/bin/ts_calibrate");
    exit(0);
}
void CSysCfgForm::OnButtonClick(skin_item_t* item)
{
    if(item->id == _skinBtns[0]->GetId())
    {
            CTimer TimeSet_Menu;
            TimeSet_Menu.CreateForm(m_hWnd);
    }
    else if(item->id == _skinBtns[1]->GetId())
    {
            CIPset IpSet_Menu;
            IpSet_Menu.CreateForm(m_hWnd);
    }
    else if(item->id == _skinBtns[2]->GetId())
    {
            CSysLogForm sys;
            sys.CreateForm(m_hWnd);

    }
    else if(item->id == _skinBtns[3]->GetId())
    {
            CUISetForm ui;
            ui.CreateForm (m_hWnd);
    }
    else if(item->id == _skinBtns[4]->GetId())
    {
            Close();
    }else if(item->id == _skinBtns[5]->GetId())
    {
            calib_touch_screen();
    }

}
