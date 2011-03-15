#include "LoginFormItem.h"
#include "LoginForm.h"
#include "BmpRes.h"
#include <yatengine.h>
#include "SoftKeyboard.h"
#include "MainMenu.h"
#include "tajidbmgr.h"

SoftKeyboard *ime;
using namespace TelEngine;
static COMM_CTRL_DESC commctrls[] = {
    EDIT_USERNAME,
    EDIT_PASSWORD
};
static SKIN_BUTTON_DESC skinctrls[] = {
    BUTTON_LOGIN
};
static const char *mmenu_bmps[] = {
        PLOGIN_BACKGROUND,
        PLOGIN_OK_BTN
};

static skin_item_t Skins[] = {
     {
	 BUTTON_OK_ID,
	 BUTTON_OK_STYLE,
	 BUTTON_OK_X,
	 BUTTON_OK_Y,
	 BUTTON_OK_ITEMRC,
	 BUTTON_OK_BMPINDEX,
	 BUTTON_OK_TIP,
	 BUTTON_OK_ATTACHED,
         BUTTON_OK_DATA
     }
};
static const char *icon_path[] = {
        PUSER_LOGIN_LABLE,
        PCOMM_EDIT_180_BKGROUND,
        PCOMM_EDIT_180_BKGROUND,
        PUSER_VERSION_LABLE
};
static const char *msg_path[] = {
        PUSER_PWD_ERR_MSG,
        PUSER_PROMOTE_MSG
};

CLoginForm::CLoginForm()
{
    m_msg_delay = 0;

    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
            printf("Can't loadres\n");
            exit(0);
    }
    _edtUser = new CEdit(&commctrls[0],this);
    _edtPwd  = new CEdit(&commctrls[1],this);
    _edtPwd->EnablePassword(true);
    _btnLogin = new CSkinButton(&skinctrls[0],this);

    _lables.AddIcons(icon_path,ARRAY_SIZE(icon_path,char*));
    _msg.AddIcons(msg_path,ARRAY_SIZE(msg_path,char*));

    InitSkinHeader("CLoginForm");

}

CLoginForm::~CLoginForm()
{

}
void CLoginForm::handle1(ADNotification* pNf)
{
    poco_check_ptr (pNf);
    AutoPtr<ADNotification> nf = pNf;
    TAdQueue *que = nf->Message();

    while(que->size() > 0)
    {
        TAD ad = que->front();
        que->pop();
        fprintf(stderr,"GetAd type = %d value = %d\n",ad.m_type,ad.m_value);
    }

}
int  imeHookProc(HWND hwnd, int msg , WPARAM w, LPARAM l)
{
    if(msg == MSG_LBUTTONUP)
    {
        ime->T9_Show(true);
    }else if(msg == MSG_KILLFOCUS){
        ime->T9_Show(false);
    }
    //fprintf(stderr,"recv %d\n",msg);
    return 0;
}
void
CLoginForm::OnCreate()
{
    SetTimer(m_hWnd,100,100);
    //this->RegisterEditHook(imeHookProc);
    //CDataAcquire::Get().addObserver(Notifyer(CLoginForm,ADNotification,handle1));
}
void CLoginForm::OnUserMsg(HWND hWnd, int message, WPARAM wParam,
                                           LPARAM lParam)
{

}
void
CLoginForm::OnShow()
{
    _edtUser->SetLimitLength(16);
    _edtPwd->SetLimitLength(16);
    repeat = 0;
    ime = new SoftKeyboard();
    //ime->T9_Show(false);
    //this->RegisterHotKey (m_hWnd,0,0,SCANCODE_F8);
    //_edtUser->EnableDigtal(true);
    //_edtUser->EnableDecimal(true);
}

void
CLoginForm::OnTimer(int ID)
{
    //fprintf(stderr,"CLoginForm OnTimer\n");
    if (m_msg_delay > 0)
    {
        if (--m_msg_delay <= 0)
        {

            UpdateWindow(m_hWnd, true);
            m_msg_delay = 0;
        }
    }
    if(repeat> 0)
        repeat--;
}

void CLoginForm::OnCommCtrlNotify(HWND hwnd, int id, int nc)
{

    if((EDIT_USERNAME_ID == id)&&(EN_SETFOCUS== nc))
    {
        UpdateWindow(m_hWnd, true);

        _msg.Show(MSG_ICON_X,MSG_ICON_Y,1);

        m_msg_delay = 3;
    }
}

void
CLoginForm::OnPaint(HWND hWnd)
{

    HDC hdc = BeginPaint(hWnd);

    _lables.Show(hdc,ICON_USER_POS,1);
    _lables.Show(hdc,ICON_USER_ICON_POS,2);
    _lables.Show(hdc,ICON_SEC_POS,3);
    _lables.Show(hdc,ICON_VER_POS,4);
    EndPaint(hWnd, hdc);
}

void
CLoginForm::OnClose()
{
    MainWindowCleanup(m_hWnd);
}

void
CLoginForm::OnButtonClick(skin_item_t * item)
{

      fprintf(stderr,"hello world1\n");
      if(_btnLogin->GetId() == item->id){

        fprintf(stderr,"hello world2\n");
        CMainMenu mm;
        mm.CreateForm(m_hWnd);
        /*
        CTajiDbMgr::Get().load("ctx2000.sqlite3");
        TWorkSiteParam par;
        par.alert_dist = 10;
        par.anchor_coord.at(0).x=100;
        par.anchor_coord.at(0).y=100;
        CTajiDbMgr::Get().UpdateWorksite(&par);
        TTjParam p;
        CTajiDbMgr::Get().UpdateTjParam(&p);

        CMainMenu mm;
         //ime->T9_Show(false);
        mm.CreateForm(m_hWnd);
        return;
        Message m("ext.ctrl");

        m.setParam("port",  _edtUser->GetText().c_str());
        m.setParam("output",_edtPwd->GetText().c_str() );
        if (!Engine::dispatch(m)) {
                  Debug("GPIO",DebugWarn,"ext.ctrl failed");
        }*/
        //std::cerr << "port:" << m.getValue(m.getValue("ret") << std::endl;
    }
}

void
CLoginForm::OnLButtonDown(DWORD keystatus, int x, int y)
{

}
void CLoginForm::OnKeyUp(int scancode, DWORD shiftstate)
{
    if(scancode == SCANCODE_ESCAPE)
    {

        repeat++;
        if(repeat > 2)
        {
            int ret = MessageBox(m_hWnd,"是否重启称重软件?","    提示    ",MB_YESNO);
            if(ret == IDYES)
            {
                Close();
            }else
            {
                repeat=0;
            }
        }
    }
    else if(scancode==SCANCODE_F1){
        fprintf(stderr,"here12\n");
        ime->T9_Show(true);
    }
}

bool CLoginForm::CheckPWD(HWND hWnd)
{
    string name = _edtUser->GetText();
    string pwd  = _edtPwd->GetText();

    return true;
}
