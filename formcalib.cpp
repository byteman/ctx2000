#include "formcalib.h"
#include "formcalibitem.h"
#include "BmpRes.h"

static const char* mmenu_bmps[] = {
     PCOMM_BACKGROUND,
     PSETZERO_BTN,
     PCALB_BTN,
     PCOMM_SAVE_BTN,
     PCOMM_CLOSE_BTN,
     PCOMM_RET_BTN,
     PMODIFY_BTN,  //new add button for line k
     PREAD_BTN,  //new add button for read line k
};
static COMM_CTRL_DESC CommCtrls[] =
{
    EDIT_CUR_AD,
    EDIT_START_X,
    EDIT_START_Y,
    EDIT_END_X,
    EDIT_END_Y,
    EDIT_START_AD,
    EDIT_END_AD,

};
static COMM_CTRL_DESC StaticCtrls[] =
{

    STATIC_CUR_AD,
    STATIC_START_X,
    STATIC_START_Y,
    STATIC_END_X,
    STATIC_END_Y,
    STATIC_START_AD,
    STATIC_END_AD,
};
static SKIN_CTRL_DESC SkinCtrls[] = {
    BUTTON_CABZERO,
    BUTTON_CABK,
    BUTTON_SAVE,
    BUTTON_EXIT,
    BUTTON_RET_CALIB,
    BUTTON_LINEK,
    BUTTON_READ_LINEK
};
#define MSG_AD MSG_USER+0x100
class ADMessageHandler:public MessageHandler{
public:
    ADMessageHandler(HWND handle) :
        MessageHandler("ad.angle"),
        m_handle(handle)
    {

    }
    virtual bool received(Message &msg)
    {
        int angle = msg.getIntValue("value");
        PostMessage(m_handle,MSG_AD,(WPARAM)angle,0);
    }
private:
    HWND m_handle;
};
CFormCalib::CFormCalib()
{
    if(!LoadRes(&mmenu_bmps[0],ARRAY_SIZE(mmenu_bmps,char*)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
    for(int i = 0 ;i < (sizeof(SkinCtrls)/sizeof(SKIN_CTRL_DESC));i++)
    {
        _btns[i] = new CSkinButton(&SkinCtrls[i],this);
    }
    for(int i = 0 ;i < (sizeof(CommCtrls)/sizeof(COMM_CTRL_DESC));i++)
    {
        _edits[i] = new CEdit(&CommCtrls[i],this);
    }

    for(int i = 0 ;i < (sizeof(StaticCtrls)/sizeof(COMM_CTRL_DESC));i++)
    {
        _lables[i] = new CStatic(&StaticCtrls[i],this);
    }



    InitSkinHeader("formcalib");
}

CFormCalib::~CFormCalib()
{

}

void CFormCalib::OnCreate()
{
    msgHandler = new ADMessageHandler(m_hWnd);
    TelEngine::Engine::install(msgHandler);
}
void CFormCalib::OnClose()
{
    TelEngine::Engine::uninstall(msgHandler);
}
void CFormCalib::OnTimer(int ID)
{

}
void CFormCalib::OnUserMsg(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    if( (hWnd == m_hWnd) && (message == MSG_AD) )
    {
        int angle_ad = (int)wParam;
        _edits[0]->SetIntText(angle_ad);
    }
}
void CFormCalib::OnPaint(HWND hWnd)
{

}
void CFormCalib::OnShow()
{

}
void CFormCalib::OnButtonClick(skin_item_t* item)
{

}
