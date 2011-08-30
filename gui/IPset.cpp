#include "IPsetItem.h"
#include "IPset.h"

#include "BmpRes.h"
#include "iniFile.h"


static COMM_CTRL_DESC commctrls[] =
{
    CENTER_EDIT_PORT,
    MEDIA_EDIT_PORT,
    DEV_ID_EDIT,
    DEV_NAME_EDIT
};

static COMM_CTRL_DESC ipeditctrls[] =
{
    CENTER_IPEDIT,
    MEDIA_IPEDIT,
    ETH0IP_IPEDIT,
    ETH0MASK_IPEDIT,
    ETH0GATE_IPEDIT,
    ETH0DNS_IPEDIT/*,del gprs begin
	PPP0IP_IPEDIT,
	PPP0MASK_IPEDIT,
	PPP0GATE_IPEDIT,
	PPP0DNS_IPEDIT del gprs end*/
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
    SKIN_BUTTON_CANCEL,
    SKIN_BUTTON_EXIT
};

static SKIN_CHECK_DESC SkinChksDesc[] =
{
    SKIN_CHECK_ETH0
//	SKIN_CHECK_PPP0
};

static const char *icon_path[] =
{
    SYSCFG_SITE_ICON,
    SYSCFG_SITE_LABEL,
    PCOMM_LINE_ICON,
    SYSCFG_IPSET_TEXT,
    SYSCFG_IPSET_CENTER_IP,
    SYSCFG_IPSET_CENTER_PORT,
    PCOMM_EDIT_180_BKGROUND,
    SYSCFG_IPSET_MEDIA_IP,
    SYSCFG_IPSET_MEDIA_PORT,
    PCOMM_EDIT_180_BKGROUND,
    SYSCFG_IPSET_ETH0,
//	SYSCFG_IPSET_PPP0,
    SYSCFG_IPSET_ETH0_IP,
    SYSCFG_IPSET_ETH0_MASK,
    SYSCFG_IPSET_ETH0_GATEWAY,
    SYSCFG_IPSET_ETH0_DNS,
    SYSCFG_IPSET_DEV_ID,
    SYSCFG_IPSET_DEV_NAME,
    PCOMM_EDIT_180_BKGROUND,
    PCOMM_EDIT_180_BKGROUND
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
        CENTER_PORT_X,
        CENTER_PORT_Y
    },
    {
        CENTER_PORTEDIT_X,
        CENTER_PORTEDIT_Y
    },
    {
        MEDIA_IP_X,
        MEDIA_IP_Y
    },
    {
        MEDIA_PORT_X,
        MEDIA_PORT_Y
    },
    {
        MEDIA_PORTEDIT_X,
        MEDIA_PORTEDIT_Y
    },
    {
        ETH0_X,
        ETH0_Y
    },
//	{
//	PPP0_X,
//	PPP0_Y
//	},
    {
        ETH0_IP_X,
        ETH0_IP_Y
    },
    {
        ETH0_MASK_X,
        ETH0_MASK_Y
    },
    {
        ETH0_GATEWAY_X,
        ETH0_GATEWAY_Y
    },
    {
        ETH0_DNS_X,
        ETH0_DNS_Y
    },
    {
        DEV_ID_X,
        DEV_ID_Y
    },
    {
        DEV_NAME_X,
        DEV_NAME_Y
    },
    {
        DEV_IPEDIT_X,
        DEV_IPEDIT_Y
    },
    {
        DEV_NAMEEDIT_X,
        DEV_NAMEEDIT_Y
    }
    /*, del gprs begin
    {
    PPP0_IP_X,
    PPP0_IP_Y
    },
    {
    PPP0_MASK_X,
    PPP0_MASK_Y
    },
    {
    PPP0_GATEWAY_X,
    PPP0_GATEWAY_Y
    },
    {
    PPP0_DNS_X,
    PPP0_DNS_Y
    } del gprs end*/
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

    for (int i = 0; i < IPSET_EDIT_NUM; i++)
        _edit[i] = new CEdit(&commctrls[i],this);

    for (int j = 0; j < IPSET_BTN_NUM; j++)
        _skinBtns[j] = new CSkinButton(&SkinBtnsDesc[j],this);

    for (int k = 0; k < IPSET_CHECK_NUM; k++)
        _skinChks[k] = new CSkinChkButton(&SkinChksDesc[k],this);

    for (int m = 0; m < IPSET_IPEDIT_NUM; m++)
        _ipedit[m] = new CIpAddr(&ipeditctrls[m],this);

    /*	addr1=192,addr2=168,addr3=0,addr4=111;
    	mask1=255,mask2=255,mask3=255,mask4=0;
    	gate1=192,gate2=168,gate3=0,gate4=1;
    	dns1=192,dns2=168,dns3=0,dns4=1;
    	m_bChanged = false;*/

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
    for (int i = 0; i< SKIN_BMP_NUM; i++)
        _lables.Show(hdc,IconsPos[i].x,IconsPos[i].y,i+1);
    /*del gprs begin
    	for (int i = 16; i< SKIN_BMP_NUM; i++)
    		_lables.Show(hdc,IconsPos[i].x,IconsPos[i].y,i-3);
    del gprs end*/
    EndPaint(hWnd, hdc);
}

void    CIPset::OnClose()
{

}

void
CIPset::OnShow()
{
    _edit[0]->SetLimitLength(6);
    _edit[0]->EnableDigtal(true);
    _edit[1]->SetLimitLength(6);
    _edit[1]->EnableDigtal(true);
    _edit[2]->SetLimitLength(10);
    _edit[3]->SetLimitLength(10);
    _skinChks[0]->SetCheckStatus(true);
    TIniFile* center_fp = new TIniFile(DT1000CONF);
    if(center_fp)
    {
        string center_ip = center_fp->ReadString("dataCenter","ip","127.0.0.1");
        _ipedit[0]->setAddress(center_ip);
        string center_port = center_fp->ReadString("dataCenter","port","5001");
        _edit[0]->SetText(center_port);
        string dev_id = center_fp->ReadString("devInfo","devId","1234567890");
        _edit[2]->SetText(dev_id);
        string dev_name = center_fp->ReadString("devInfo","devName","DT12345678");
        _edit[3]->SetText(dev_name);
        delete center_fp;
    }
    TIniFile* media_fp = new TIniFile(VIDEOSERVERCONF);
    if(media_fp)
    {
        string media_ip = media_fp->ReadString("server","addr","192.168.0.3");
        _ipedit[1]->setAddress(media_ip);
        string media_port = media_fp->ReadString("server","port","5000");
        _edit[1]->SetText(media_port);
        delete media_fp;
    }
    TIniFile* rcconf_fp = new TIniFile(RCCONF);
    if(rcconf_fp)
    {
        string ipaddr0 = rcconf_fp->ReadString("IPADDR0","127.0.0.1");
        _ipedit[2]->setAddress(ipaddr0);
        string netmask0 = rcconf_fp->ReadString("NETMASK0","255.255.255.0");
        _ipedit[3]->setAddress(netmask0);
        string gateway0 = rcconf_fp->ReadString("GATEWAY0","127.0.0.1");
        _ipedit[4]->setAddress(gateway0);
        string nameserver0 = rcconf_fp->ReadString("NAMESERVER0","127.0.0.1");
        _ipedit[5]->setAddress(nameserver0);
        delete rcconf_fp;
    }
}

void    CIPset::OnButtonClick(skin_item_t* item)
{
    if(item->id == _skinChks[0]->GetId())
    {
//		_skinChks[0]->SetCheckStatus(true);
//		_skinChks[1]->SetCheckStatus(false);
//    }
//    else if(item->id == _skinChks[1]->GetId()){
//		/* del gprs begin
//		_skinChks[0]->SetCheckStatus(false);
//		_skinChks[1]->SetCheckStatus(true);
//		 del gprs end*/
//		_skinChks[1]->SetCheckStatus(false);
    }
    else if(item->id == _skinBtns[0]->GetId())
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
    string center_ip;
    string center_port;
    string media_ip;
    string media_port;
    string ipaddr0;
    string netmask0;
    string broadcast0;
    string gateway0;
    string nameserver0;
    string dev_id;
    string dev_name;
    size_t found;


    center_ip   = _ipedit[0]->getAddress();

    center_port = _edit[0]->GetText();
    media_ip    = _ipedit[1]->getAddress();
    media_port  = _edit[1]->GetText();
    ipaddr0     = _ipedit[2]->getAddress();
    netmask0    = _ipedit[3]->getAddress();
    found = ipaddr0.find_last_of('.');
    broadcast0  = ipaddr0.substr(0,found)+".255";
    gateway0    = _ipedit[4]->getAddress();
    nameserver0 = _ipedit[5]->getAddress();
    dev_id      = _edit[2]->GetText();
    dev_name    = _edit[3]->GetText();

    if((0==center_port.length())||(0==media_port.length()))
    {
        PromoteErrMsg(0);
        return 1;
    }
    else
    {

        TIniFile centerCfg(DT1000CONF);

        centerCfg.WriteString("dataCenter","ip", center_ip);
        centerCfg.WriteString("dataCenter","port", center_port);
        centerCfg.WriteString("devInfo","devId", dev_id);
        centerCfg.WriteString("devInfo","devName",dev_name);
        centerCfg.WriteString("devInfo","localIp",ipaddr0);
//======================================================================
        TIniFile mediaCfg(VIDEOSERVERCONF);

        mediaCfg.WriteString("server","addr",media_ip);
        mediaCfg.WriteString("server","port",media_port);
//======================================================================
        TIniFile rcconfCfg(RCCONF);

        rcconfCfg.WriteString("IPADDR0", ipaddr0);
        rcconfCfg.WriteString("NETMASK0", netmask0);
        rcconfCfg.WriteString("GATEWAY0", gateway0);
        rcconfCfg.WriteString("BROADCAST0", broadcast0);
        rcconfCfg.WriteString("NAMESERVER0", nameserver0);
        rcconfCfg.WriteString("NTP_SERVER", center_ip);

        return 0;
    }
    return 4;	//sth wrong*/
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
