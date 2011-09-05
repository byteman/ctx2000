#include "IPsetItem.h"
#include "IPset.h"
#include "BmpRes.h"
#include "iniFile.h"
#include "comdata.h"
#include <Poco/Format.h>
static COMM_CTRL_DESC commctrls[] =
{
    CENTER_IPEDIT,
    CENTER_EDIT_PORT,
    DEV_ID_EDIT
};

static COMM_CTRL_DESC ipeditctrls[] =
{

};
static const char* mmenu_bmps[] =
{
    PCOMM_BACKGROUND,
    PCOMM_OK_BTN,
    PCOMM_CANCEL_BTN,
    PCOMM_CLOSE_BTN,
    PCOMM_CHECK_BTN
};

static SKIN_BUTTON_DESC SkinBtnsDesc[] =
{
    SKIN_BUTTON_OK,
    //SKIN_BUTTON_CANCEL,
    SKIN_BUTTON_EXIT
};

static SKIN_CHECK_DESC SkinChksDesc[] =
{

};

static const char *icon_path[] =
{
    SYSCFG_SITE_ICON,
    SYSCFG_SITE_LABEL,
    PCOMM_LINE_ICON,
    SYSCFG_IPSET_TEXT,
    SYSCFG_IPSET_CENTER_IP,
    PCOMM_EDIT_180_BKGROUND,
    SYSCFG_IPSET_CENTER_PORT,
    PCOMM_EDIT_180_BKGROUND,
    SYSCFG_IPSET_DEV_ID,
    PCOMM_EDIT_250_BKGROUND,
};

static POS IconsPos[] =
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
        LINE_X,
        LINE_Y
    },
    {
        SYSCFG_IPSET_X,
        SYSCFG_IPSET_Y
    },
    {
        CENTER_IP_X,
        CENTER_IP_Y
    },
    {
        CENTER_IP_X+150,
        CENTER_IP_Y-10
    },
    {
        CENTER_PORT_X,
        CENTER_PORT_Y
    },
    {
        CENTER_PORT_X+150,
        CENTER_PORT_Y-10
    },
    {
        DEV_ID_X,
        DEV_ID_Y
    },
    {
        DEV_ID_X+150,
        DEV_ID_Y-10
    },
};

static const char *msg_path[] =
{
    SYSCFG_IPSET_BLANK_MSG
};

static POS Msgs[] =
{
    {
        BLANK_X,
        BLANK_Y
    }
};

CIPset::CIPset()
{
    m_msg_delay = 0;
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }

    _lables.AddIcons(icon_path,ARRAY_SIZE(icon_path,char*));
    _msgs.AddIcons(msg_path,ARRAY_SIZE(msg_path,char*));

    for (int i = 0; i < TABLESIZE(commctrls); i++)
        _edit[i] = new CEdit(&commctrls[i],this);

    for (int j = 0; j < TABLESIZE(SkinBtnsDesc); j++)
        _skinBtns[j] = new CSkinButton(&SkinBtnsDesc[j],this);
    InitSkinHeader("CIPset");


}

CIPset::~CIPset()
{
    KillTimer(m_hWnd,100);

}

void    CIPset::OnCreate()
{
    SetTimer(m_hWnd,100,100);
}
void    CIPset::OnTimer(int ID)
{
    if (m_msg_delay > 0)
    {
        if (--m_msg_delay <= 0)
        {
            UpdateWindow(m_hWnd, true);
            m_msg_delay = 0;
        }
    }
}

void    CIPset::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);
    for (int i = 0; i< TABLESIZE(IconsPos); i++)
        _lables.Show(hdc,IconsPos[i].x,IconsPos[i].y,i+1);
    EndPaint(hWnd, hdc);
}

void    CIPset::OnClose()
{

}

void
CIPset::OnShow()
{
    _edit[0]->SetLimitLength(15);
    _edit[1]->SetLimitLength(6);
    _edit[2]->SetLimitLength(15);

    _edit[0]->SetText (gprs_remote_ip);
    _edit[1]->SetText (Poco::format ("%d",gprs_remote_port));
    if(gprs_dtu_id.length ()<15){
        gprs_dtu_id.append (15-gprs_dtu_id.length (),0x30);
    }
    _edit[2]->SetText (gprs_dtu_id);
}

void    CIPset::OnButtonClick(skin_item_t* item)
{

    if(item->id == _skinBtns[0]->GetId())
    {

        if(0==CheckIPset())
            Close();
    }
    else
    {
        Close();
    }
}
void CIPset::PromoteErrMsg(int type)
{
    UpdateWindow(m_hWnd, true);
    _msgs.Show(Msgs[type].x,Msgs[type].y,type+1);
    m_msg_delay = 3;
}

int CIPset::CheckIPset()
{
    TIniFile cfg(ctx2000_cfg);
    cfg.WriteString ("gprs","ip",    _edit[0]->GetText ());
    cfg.WriteString ("gprs","port",  _edit[1]->GetText ());
    cfg.WriteString ("gprs","dtuid", _edit[2]->GetText ());
    return 0;
}
#include "SoftKeyboard.h"
extern SoftKeyboard* skt;
void   CIPset::OnCommCtrlNotify(HWND hwnd, int id, int nc)
{
    if(nc==EN_SETFOCUS)
    {
        fprintf(stderr,"setfocus\n");
        if(skt)
        {
            //fprintf(stderr,"t9show\n");
            skt->T9_Show(true);
        }
    }
}
