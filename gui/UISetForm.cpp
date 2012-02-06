#include "UISetForm.h"
#include "BmpRes.h"
#include "iniFile.h"
#include "comdata.h"
#include "resStr.h"
static const char *SPFormBmps[] =
{
    PCOMM_BACKGROUND,
    PCOMM_CHECK_BTN,
    PCOMM_CHECK_BTN,
    PCOMM_SAVE_BTN,
    PCOMM_CLOSE_BTN,
    "comm/btn_upload.png"
};
static std::string lables[] = {
    "显示重量:",
    "显示高度:",
    "显示风速:",
    "多机模式:",
    "单机模式",
    "风速13m/s报警",
    "风速20m/s报警",
    "中文界面",
    "英文界面"
};

#define X_POS  300
#define Y_POS  100
#define X_POS1  600
#define V_SPAN 60
static SKIN_CHECK_DESC  SkinCheckDesc[]= {

    {2, X_POS, Y_POS, NULL}, //show weight
    {2, X_POS, Y_POS+V_SPAN, NULL}, //show height
    {2, X_POS, Y_POS+2*V_SPAN, NULL}, //show speed
    {2, X_POS, Y_POS+3*V_SPAN, NULL}, //show multi
    {2, X_POS, Y_POS+4*V_SPAN, NULL}, //show single

    {2, X_POS1, Y_POS, NULL}, //show 13m/s
    {2, X_POS1, Y_POS+V_SPAN, NULL}, //show 20m/s
    {2, X_POS1, Y_POS+2*V_SPAN, NULL}, //show chinese
    {2, X_POS1, Y_POS+3*V_SPAN, NULL}, //show english
};

static SKIN_BUTTON_DESC SkinBtnsDesc[] = {
    {3,250,418, NULL},
    {4,738,8,   NULL},
    {5,500,418,   NULL},
};

#define SKIN_BMP_NUM 3
static const char *icon_path[] =
{

};

static POS BmpsPos[] = {
    {30,22},
    {134,33},
    {155,25}
};

/*
    "显示重量:",
    "显示高度:",
    "显示风速:",
    "多机模式:",
    "单机模式",
    "风速13m/s报警",
    "风速20m/s报警",
    "中文界面",
    "英文界面"
*/
static void loadRes()
{
    lables[0] = CResStr::Get ().at (res_show_weight);
    lables[1] = CResStr::Get ().at (res_show_height);
    lables[2] = CResStr::Get ().at (res_show_wild);
    lables[3] = CResStr::Get ().at (res_multi_mode);
    lables[4] = CResStr::Get ().at (res_signal_mode);
    lables[5] = CResStr::Get ().at (res_wild_13_alarm);
    lables[6] = CResStr::Get ().at (res_wild_20_alarm);
    lables[7] = CResStr::Get ().at (res_lang_zh);
    lables[8] = CResStr::Get ().at (res_lang_en);
}
CUISetForm::CUISetForm()
{
    int buttonCount = TABLESIZE(SPFormBmps);

    if (!LoadRes(&SPFormBmps[0], buttonCount)) {
        printf("Can't loadres\n");
        exit(0);
    }

    for(int i=0;i<TABLESIZE(SkinCheckDesc);i++) {
        _skinChk[i] = new CSkinChkButton(&SkinCheckDesc[i],this);
    }

    _saveButton = new CSkinButton(&SkinBtnsDesc[0], this);
    _exitButton = new CSkinButton(&SkinBtnsDesc[1],this);
    _updateButton = new CSkinButton(&SkinBtnsDesc[2],this);
    _icons.AddIcons(icon_path,ARRAY_SIZE(icon_path,char*));

    Font16 = CFontMgr::Get().GetFont("stxinwei",24);
    Font16->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));
#define LEFT 250
    for(int i = 0; i < 5; i++)
    {
        SetRect(&m_lable[i],        X_POS-LEFT,Y_POS+i*V_SPAN,  X_POS+80,Y_POS+i*V_SPAN+30);
    }
    for(int i = 5; i < TABLESIZE(lables); i++)
    {
        SetRect(&m_lable[i],        X_POS1-LEFT,Y_POS+(i-5)*V_SPAN,  X_POS1+80,Y_POS+(i-5)*V_SPAN+30);
    }
    InitSkinHeader("CUISetForm");
    loadRes();
}

CUISetForm::~CUISetForm()
{
}
#include "MsgBox.h"
void CUISetForm::updatefile()
{
    int ret = system ("./usb_update.sh");
    fprintf(stderr,"usb_update.sh = %d\n",ret);
    if(0==ret)
    {
        MsgBox box;
        box.ShowBox (this,"update file ok,please reset","hint");
    }else if(256==ret){
        MsgBox box;
        box.ShowBox (this,"do not detect valid update file","hint");
    }else if(512==ret){
        MsgBox box;
        box.ShowBox (this,"do not detect usb","hint");
    }
}
void CUISetForm::OnButtonClick(skin_item_t *item)
{
    if(item->id == _saveButton->GetId())
    {
        savedata();
        Close();
    }
    else if (item->id == _exitButton->GetId())
    {
        Close();
    }else if (item->id == _updateButton->GetId ()){
        updatefile();
    }else if(item->id == _skinChk[3]->GetId()){

        bool ok = _skinChk[3]->GetCheckStatus();

        if(ok){
            _skinChk[4]->SetCheckStatus(false);
        }else{
            _skinChk[4]->SetCheckStatus(true);
        }

    }else if(item->id == _skinChk[4]->GetId()){
        bool ok = _skinChk[4]->GetCheckStatus();
        if(ok){
            _skinChk[3]->SetCheckStatus(false);
        }else{
            _skinChk[3]->SetCheckStatus(true);
        }
    }else if(item->id == _skinChk[5]->GetId()){
        bool ok = _skinChk[5]->GetCheckStatus();
        if(ok){
            _skinChk[6]->SetCheckStatus(false);
        }else{
            _skinChk[6]->SetCheckStatus(true);
        }
    }else if(item->id == _skinChk[6]->GetId()){
        bool ok = _skinChk[6]->GetCheckStatus();
        if(ok){
            _skinChk[5]->SetCheckStatus(false);
        }else{
            _skinChk[5]->SetCheckStatus(true);
        }
    }else if(item->id == _skinChk[7]->GetId()){
        bool ok = _skinChk[7]->GetCheckStatus();
        if(ok){
            _skinChk[8]->SetCheckStatus(false);
        }else{
            _skinChk[8]->SetCheckStatus(true);
        }
    }else if(item->id == _skinChk[8]->GetId()){
        bool ok = _skinChk[8]->GetCheckStatus();
        if(ok){
            _skinChk[7]->SetCheckStatus(false);
        }else{
            _skinChk[7]->SetCheckStatus(true);
        }
    }
}

void CUISetForm::OnShow()
{
    TIniFile cfg(ctx2000_cfg);
    if(cfg.ReadBool("display","weight",   false))
    {
        _skinChk[0]->SetCheckStatus (true);
    }else{
        _skinChk[0]->SetCheckStatus (false);
    }
    if(cfg.ReadBool("display","height",   false))
    {
        _skinChk[1]->SetCheckStatus (true);
    }else{
        _skinChk[1]->SetCheckStatus (false);
    }
    if(cfg.ReadBool("display","speed",  false))
    {
        _skinChk[2]->SetCheckStatus (true);
    }else{
        _skinChk[2]->SetCheckStatus (false);
    }
    if(cfg.ReadBool("display","lang",  false))
    {
        _skinChk[7]->SetCheckStatus (true);
        _skinChk[8]->SetCheckStatus (false);
    }else{
        _skinChk[8]->SetCheckStatus (true);
        _skinChk[7]->SetCheckStatus (false);
    }
    m_cur_mode = cfg.ReadInteger("display","mainmenu",1);
    if(m_cur_mode == 1)
    {
        _skinChk[3]->SetCheckStatus(true);
        _skinChk[4]->SetCheckStatus(false);
    }else if(m_cur_mode == 2)
    {
        _skinChk[4]->SetCheckStatus(true);
        _skinChk[3]->SetCheckStatus(false);
    }
    if(g_alarm_wild_speed==13){
        _skinChk[5]->SetCheckStatus(true);
        _skinChk[6]->SetCheckStatus(false);
    }else{
        _skinChk[6]->SetCheckStatus(true);
        _skinChk[5]->SetCheckStatus(false);
    }
}
void CUISetForm::savedata()
{

    if(_skinChk[3]->GetCheckStatus()){

        if(m_cur_mode==1)
         {}
        else
            m_cur_mode = 1;
    }

    if(_skinChk[4]->GetCheckStatus()){

        if(m_cur_mode==2)
         {}
         else
            m_cur_mode = 2;
    }

    TIniFile cfg(ctx2000_cfg);
    cfg.WriteBool ("display","weight",_skinChk[0]->GetCheckStatus ());
    cfg.WriteBool ("display","height",_skinChk[1]->GetCheckStatus ());
    cfg.WriteBool ("display","speed",_skinChk[2]->GetCheckStatus ());
    cfg.WriteInteger("display","mainmenu",m_cur_mode);

    if(_skinChk[5]->GetCheckStatus ()){
        cfg.WriteInteger("display","alarm_speed",13);
        g_alarm_wild_speed = 13;
    }else {
        if(_skinChk[6]->GetCheckStatus ()){
            cfg.WriteInteger("display","alarm_speed",20);
            g_alarm_wild_speed = 20;
        }
    }

    if(_skinChk[7]->GetCheckStatus ()){
        cfg.WriteBool ("display","lang",true);
    }else {
        if(_skinChk[8]->GetCheckStatus ()){
            cfg.WriteInteger("display","lang",false);
        }
    }
}
void CUISetForm::OnClose()
{

}

void CUISetForm::OnPaint(HWND hWnd)
{

     HDC hdc = BeginPaint(hWnd);

     for(int i = 0; i < TABLESIZE(lables); i ++)
       DrawMyText(hdc,Font16,&m_lable[i],lables[i]);
     EndPaint(hWnd, hdc);
}

