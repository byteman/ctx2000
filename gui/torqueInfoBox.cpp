#include "torqueInfoBox.h"
#include "torqueInfoBoxItem.h"
#include "BmpRes.h"


static COMM_CTRL_DESC commctrls[] =
{
    EDIT_MINETYPENAME,
    EDIT_MINEPRICE
};

static const char* mmenu_bmps[] =
{
    "comm/lijubk.png",
    PCOMM_OK_BTN,
    PCOMM_CANCEL_BTN,
    PCOMM_CLOSE_BTN
};

static SKIN_BUTTON_DESC SkinBtnsDesc[] =
{
    SKIN_BUTTON_OK,
    SKIN_BUTTON_CANCEL,
    SKIN_BUTTON_EXIT
};

static const char *icon_path[] =
{
    PPRESET_SITE_ICON,
    PPRESET_SITE_LABEL,
    PCOMM_EDIT_BKGROUND,
    PCOMM_EDIT_BKGROUND
};

static POS IconsPos[] =
{
    {
        SYSMT_X,
        SYSMT_Y
    },

    {
        SYSMT_TEXT_X,
        SYSMT_TEXT_Y
    },
    {
        BORDER_MINETYPENAME_X,
        BORDER_MINETYPENAME_Y
    },
    {
        BORDER_MINEPRICE_X,
        BORDER_MINEPRICE_Y
    }
};

static const char *msg_path[] =
{
    PPRESET_MINEMAN_BLANK_MSG,
    PPRESET_MINEMAN_DUPMINETYPE_MSG,
    PPRESET_MINEMAN_MINETYPEMSG_MSG
};

static POS Msgs[] =
{
    {
        BLANK_X,
        BLANK_Y
    },
    {
        DUPMINETYPE_X,
        DUPMINETYPE_Y
    },
    {
        MINETYPEMSG_X,
        MINETYPEMSG_Y
    }
};
#include "SoftKeyboard.h"
extern SoftKeyboard* skt;
void  CTorqueInfoBox::OnCommCtrlNotify(HWND hwnd, int id, int nc)
{
    if(nc==EN_SETFOCUS)
    {
        fprintf(stderr,"setfocus\n");
#if 1
        if(skt)
        {
            skt->T9_Show(true);
        }
#endif
    }
}
CTorqueInfoBox::CTorqueInfoBox()
{
    m_msg_delay = 0;
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }

    for (int i = 0; i < MINEADD_EDIT_NUM; i++)
        _edit[i] = new CEdit(&commctrls[i],this);

    for (int j = 0; j < MINEADD_BTN_NUM; j++)
        _skinBtns[j] = new CSkinButton(&SkinBtnsDesc[j],this);

    _lables.AddIcons(icon_path,ARRAY_SIZE(icon_path,char*));
    _msgs.AddIcons(msg_path,ARRAY_SIZE(msg_path,char*));

    InitSkinHeader("CTorqueInfoBox");
}

CTorqueInfoBox::~CTorqueInfoBox()
{
    KillTimer(m_hWnd,100);

}

void    CTorqueInfoBox::OnCreate()
{
    SetTimer(m_hWnd,100,100);
}
void    CTorqueInfoBox::OnTimer(int ID)
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

void    CTorqueInfoBox::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);

    for (int i = 0; i< 6; i++)
        _lables.Show(hdc,IconsPos[i].x,IconsPos[i].y,i+1);
    for (int i = 6; i< SKIN_BMP_NUM; i++)
        _lables.Show(hdc,IconsPos[i].x,IconsPos[i].y,7);

    EndPaint(hWnd, hdc);
}

void    CTorqueInfoBox::OnClose()
{

}

void
CTorqueInfoBox::OnShow()
{
    m_bOk = false;
    _edit[0]->SetLimitLength(16);
    _edit[1]->SetLimitLength(10);
    _edit[0]->SetText(m_arm_len);
    _edit[1]->SetText(m_weight);
}

void    CTorqueInfoBox::OnButtonClick(skin_item_t* item)
{
    if(item->id == _skinBtns[0]->GetId())
    {
        m_arm_len = _edit[0]->GetText();
        m_weight  = _edit[1]->GetText();
        m_bOk = true;
        Close();

    }
    else
    {
        m_bOk = false;
        Close();

    }
}
