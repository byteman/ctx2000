#include "Password.h"

#define MSG_BJ_W     300
#define MSG_BJ_H     172
#define MARGIN_V       50
#define MARGIN_H       70
#define PASS_EDT_W     180
#define PASS_EDT_H     30
static PLOGFONT gInfoFont  = INV_LOGFONT;
static PLOGFONT gTitleFont = INV_LOGFONT;
#define FORM_NAME "msgbox"
#include "SoftKeyboard.h"
extern SoftKeyboard *skt;
static const char* mmenu_bmps[] = {
    "ctx2000/msgboxbj.png",
    "ctx2000/ok_btn.png",
    "ctx2000/close_btn.png",
};
static SKIN_BUTTON_DESC SkinBtns[] = {
    {1,100,120},
    {2,256,5}
};
static COMM_CTRL_DESC CommDesc[] = {
    {100,  60 , PASS_EDT_W,PASS_EDT_H,""},
};

PassWord::PassWord()
{
    _msgStr = "hello world\n";
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }

    _btnOk    = new CSkinButton(&SkinBtns[0],this);
    _btnClose = new CSkinButton(&SkinBtns[1],this);
    _edtPwd   = new CEdit(&CommDesc[0],this);
    _edtBk.LoadFile("comm/textbj180px.png");
    InitSkinHeader(FORM_NAME);
    Font24 = CFontMgr::Get().GetFont("stxinwei",16);
    assert(Font24 != NULL);
    Font24->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));
    m_ok = false;
}
PassWord::~PassWord()
{

}
bool PassWord::ShowBox(CSkinForm* parent,std::string strMsg,std::string strTitle,std::string strPassword,DWORD type)
{
    int left = (800 - MSG_BJ_W)/2;
    int top  = (480 - MSG_BJ_H)/2;
    int right= left + MSG_BJ_W;
    int bottom=top  + MSG_BJ_H;
    _msgStr   = strMsg;
    _titleStr = strTitle;
    _passwordStr = strPassword;
    this->CreateForm(parent->m_hWnd,left,top,right,bottom,TRUE);

    return m_ok;
    //this->CreateForm(parent->m_hWnd);
}
void   PassWord::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);

    SelectFont(hdc,Font24->pfont);
    SetBkMode(hdc,BM_TRANSPARENT);
    SetTextColor(hdc,RGB2Pixel(hdc,255,255,255));
    RECT rect;
    SetRect(&rect,40,70,MSG_BJ_W-20,MSG_BJ_H-40);
    DrawText(hdc,_msgStr.c_str(),_msgStr.length(),&rect,DT_LEFT|DT_WORDBREAK);

    SelectFont(hdc,Font24->pfont);
    SetTextColor(hdc,RGB2Pixel(hdc,255,255,255));

    TextOut(hdc,2,2,_titleStr.c_str());

    _edtBk.Show(hdc,CommDesc[0].x-8,CommDesc[0].y-8);
    EndPaint(hWnd,hdc);
}
void   PassWord::OnClose()
{
    if(skt)
    {
        skt->T9_Show (false);
    }
}
void   PassWord::OnShow()
{
    _edtPwd->EnableDecimal(false);
    _edtPwd->EnableDigtal(false);
}
void   PassWord::OnButtonClick(skin_item_t* item)
{
    if(item->id == _btnOk->GetId()){
        if(_edtPwd->GetText() == _passwordStr)
            m_ok = true;
        else
            m_ok = false;
        Close();
    }else if(item->id == _btnClose->GetId()){
        //Close();
    }
}

void PassWord::OnCommCtrlNotify(HWND hwnd, int id, int nc)
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
