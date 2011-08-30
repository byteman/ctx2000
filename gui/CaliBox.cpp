#include "CaliBox.h"
#include "GuiEng.h"
#include "BmpRes.h"
#include <Poco/NumberParser.h>
#include "MsgBox.h"
#define MSG_BJ_W     360
#define MSG_BJ_H     200
#define MARGIN_V       50
#define MARGIN_H       70
static PLOGFONT gInfoFont  = INV_LOGFONT;
static PLOGFONT gTitleFont = INV_LOGFONT;
#define FORM_NAME "CaliBox"
#include "SoftKeyboard.h"
extern SoftKeyboard *skt;
static const char* mmenu_bmps[] = {
    "ctx2000/msgboxbj.png",
    "ctx2000/ok_btn.png",
    "ctx2000/cancel_btn.png",
    "ctx2000/next.png",
    "ctx2000/close_btn.png"
};
#define COMM_BTN_W  100
#define OK_POS   ((MSG_BJ_W/3-100)/2)
#define CAN_POS  (OK_POS + MSG_BJ_W/3)
#define NEXT_POS (CAN_POS + MSG_BJ_W/3)
#define Y_POS  (MSG_BJ_H-40)
#define C_W  176/4
#define C_H  16
static SKIN_BUTTON_DESC SkinBtns[] = {
    {1,OK_POS,Y_POS},
    {2,CAN_POS,Y_POS},
    {3,NEXT_POS,Y_POS},
    {4,MSG_BJ_W-C_W,C_H},
};
static COMM_CTRL_DESC ComCtrl[] = {
    {(MSG_BJ_W-250)/2,MSG_BJ_H/2-20,180,36,"0"}
};
static const char *icon_path[] =
{
    PCOMM_EDIT_250_BKGROUND
};
CaliBox::CaliBox(int type)
{
    _msgStr = "hello world\n";
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }

    _btnOk    = new CSkinButton(&SkinBtns[0],this);
    _btnCancel  = new CSkinButton(&SkinBtns[1],this);
    if(type==1)
        _btnNext  = new CSkinButton(&SkinBtns[2],this);
    _btnClose = new CSkinButton(&SkinBtns[3],this);
    _edtInput = new CEdit(&ComCtrl[0],this);
    iconBk.LoadFile (icon_path[0]);
    InitSkinHeader(FORM_NAME);
    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
    assert(Font24 != NULL);
    Font24->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));
    m_ret = 0;
    m_input_value.clear ();
    m_type = type;
}
CaliBox::~CaliBox()
{

}
int CaliBox::ShowBox(CSkinForm* parent,std::string strMsg,std::string strTitle,DWORD type)
{
    int left = (800 - MSG_BJ_W)/2;
    int top  = (480 - MSG_BJ_H)/2;
    int right= left + MSG_BJ_W;
    int bottom=top  + MSG_BJ_H;
    _msgStr   = strMsg;
    _titleStr = strTitle;
    this->CreateForm(parent->m_hWnd,left,top,right,bottom,TRUE);
    //this->CreateForm(parent->m_hWnd);
    return m_ret;
}
void   CaliBox::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);

    SelectFont(hdc,Font24->pfont);
    SetBkMode(hdc,BM_TRANSPARENT);
    SetTextColor(hdc,RGB2Pixel(hdc,255,255,255));
    RECT rect;
    SetRect(&rect,20,40,MSG_BJ_W-20,MSG_BJ_H-40);
    DrawText(hdc,_msgStr.c_str(),_msgStr.length(),&rect,DT_CENTER|DT_WORDBREAK);

    SelectFont(hdc,Font24->pfont);
    SetTextColor(hdc,RGB2Pixel(hdc,255,255,255));

    TextOut(hdc,2,2,_titleStr.c_str());

    iconBk.Show (hdc,ComCtrl[0].x-5,ComCtrl[0].y-10,1);
    EndPaint(hWnd,hdc);
}
void   CaliBox::OnClose()
{

}
void   CaliBox::OnShow()
{

}
bool    CaliBox::check_valid(double& value)
{
    std::string str_value = _edtInput->GetText ();
    double tmp_value;
    if(Poco::NumberParser::tryParseFloat (str_value,tmp_value))
    {
        value = tmp_value;
        return true;
    }else{
        MsgBox box;
        box.ShowBox (this,"输入不正确","提示");
        return false;
    }
}
void   CaliBox::OnButtonClick(skin_item_t* item)
{
    if(item->id == _btnOk->GetId()){
        printf("ok\n");

        if(check_valid (m_double_input_value))
        {
            m_input_value = _edtInput->GetText ();
            m_ret = 1;
            Close();
        }

    }else if(item->id == _btnClose->GetId()){
        m_ret = 0;
        m_input_value = "";
        Close();
    }else if(m_type && item->id == _btnNext->GetId()){
        printf("next\n");

        if(check_valid (m_double_input_value))
        {
            m_ret = 2;
            m_input_value = _edtInput->GetText ();
            Close();
        }

    }else if(item->id == _btnCancel->GetId()){
        m_ret = 0;
        m_input_value = "";
        printf("cancel\n");
    }
}
void CaliBox::OnCommCtrlNotify(HWND hwnd, int id, int nc)
{

    if(nc==EN_SETFOCUS)
    {
        if(skt)
        {
            skt->T9_Show(true,true);
            skt->move (0,0);
        }
    }
}
