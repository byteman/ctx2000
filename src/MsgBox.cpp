#include "MsgBox.h"
#include "GuiEng.h"
#define MSG_BJ_W     300
#define MSG_BJ_H     172
#define MARGIN_V       50
#define MARGIN_H       70
static PLOGFONT gInfoFont  = INV_LOGFONT;
static PLOGFONT gTitleFont = INV_LOGFONT;
#define FORM_NAME "msgbox"

static const char* mmenu_bmps[] = {
    "ctx2000/msgboxbj.png",
    "ctx2000/ok_btn.png",
    "ctx2000/close_btn.png"
};
static SKIN_BUTTON_DESC SkinBtns[] = {
    {1,100,120},
    {2,256,5}
};

MsgBox::MsgBox()
{
    _msgStr = "hello world\n";
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }

    _btnOk    = new CSkinButton(&SkinBtns[0],this);
    _btnClose = new CSkinButton(&SkinBtns[1],this);
    InitSkinHeader(FORM_NAME);
    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
    assert(Font24 != NULL);
    Font24->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));

}
MsgBox::~MsgBox()
{

}
int MsgBox::ShowBox(CSkinForm* parent,std::string strMsg,std::string strTitle,DWORD type)
{
    int left = (800 - MSG_BJ_W)/2;
    int top  = (480 - MSG_BJ_H)/2;
    int right= left + MSG_BJ_W;
    int bottom=top  + MSG_BJ_H;
    _msgStr   = strMsg;
    _titleStr = strTitle;
    this->CreateForm(parent->m_hWnd,left,top,right,bottom,TRUE);
    //this->CreateForm(parent->m_hWnd);
}
void   MsgBox::OnPaint(HWND hWnd)
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
    EndPaint(hWnd,hdc);
}
void   MsgBox::OnClose()
{

}
void   MsgBox::OnShow()
{

}
void   MsgBox::OnButtonClick(skin_item_t* item)
{
    if(item->id == _btnOk->GetId()){
        Close();
    }else if(item->id == _btnClose->GetId()){
        Close();
    }
}
