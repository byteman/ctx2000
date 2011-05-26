// MainMenu.cpp: implementation of the CMainMenu class.
//
//////////////////////////////////////////////////////////////////////
#include "MainMenuItem.h"
#include "MainMenu.h"
#include "statusicon.h"
#include "BmpRes.h"
#include <Poco/Random.h>
#include "SysSet.h"
#include "collalgo.h"
#include "tajidbmgr.h"
#include "formworksite.h"
#include "lijuConfForm.h"
#include "iniFile.h"
#include "comdata.h"
#include "lijuctrl.h"
#include "mainctrl.h"
#include "SoftKeyboard.h"
#include <math.h>
#include <Poco/Format.h>

#define E_W  85
#define E_H  30
#define L_S  55
#define L_S_V 110
#define V_S  47

#define L_S2   695
#define L_S_V2 195

#define EDIT_ANGLE    {L_S,L_S_V+0*V_S, E_W,E_H,"20.8"}
#define EDIT_DIST     {L_S,L_S_V+1*V_S, E_W,E_H,"12.4"}
#define EDIT_MAX_W    {L_S,L_S_V+2*V_S, E_W,E_H,"12.4"}
#define EDIT_WEIGHT   {L_S,L_S_V+3*V_S, E_W,E_H,"12.4"}
#define EDIT_HEIGHT   {L_S,L_S_V+4*V_S, E_W,E_H,"12.4"}
#define EDIT_UP_ANGL  {L_S,L_S_V+5*V_S, E_W,E_H,"12.4"}
#define EDIT_BEILV    {158,352, 30,30,"3"}
#define STATIC_LIJU    {155,85,40,150,"0"}

#define EDIT_T_H      {L_S2,L_S_V2+0*V_S, E_W,E_H,"12.4"}
#define EDIT_L_ARM    {L_S2,L_S_V2+1*V_S, E_W,E_H,"12.4"}
#define EDIT_S_ARM    {L_S2,L_S_V2+2*V_S, E_W,E_H,"12.4"}
#define EDIT_SPEED    {L_S2,L_S_V2+3*V_S, E_W,E_H,"12.4"}

#define Client_Height 72


SoftKeyboard *skt=NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static const char* mmenu_bmps[] = {
        PMAINBK,
        PSCALE_BTN,
        PQUERY_BTN,
        PPRESET_BTN,
        PSYSCFG_BTN,
        PWETCFG_BTN,
        PMODIFY_BTN
};
static SKIN_BUTTON_DESC SkinBtns[] = {
    SKIN_BUTTON_WETCFG,
    SKIN_BUTTON_SYSCFG,
    SKIN_BUTTON_HELP
};
static EDevStatus g_status[20];

static COMM_CTRL_DESC areactrls[] = {
    {200,  Client_Height+0 , 450,400,""},
};

static const char* ICons[] = {

};
static COMM_CTRL_DESC commctrls[] = {
/*
    EDIT_ANGLE,
    EDIT_DIST,
    {710,100,80,30,"0"},{710,150,80,30,"0"},{710,200,80,30,"0"},
    {710,80,80,30,"H"},{710,130,80,30,"R"},{710,180,80,30,"R-"},
    EDIT_UP_ANGL, //8
    EDIT_WEIGHT, //9
    EDIT_SPEED,//10
    EDIT_HEIGHT,//11
    {710,230,80,30,"Up Angle:"},//12
    {710,285,80,30,"Max Weight:"},//13
    {710,350,80,30,"Speed:"},//14
    {710,410,80,30,"Car Height:"},//15
    {155,85,40,150,"0"}, //16
    {20,370,40,40,"1"}, //17
    {20,340,40,40,"1"} //17
 */
    EDIT_ANGLE,
    EDIT_DIST,
    EDIT_MAX_W,
    EDIT_WEIGHT,
    EDIT_HEIGHT,
    EDIT_UP_ANGL,
    EDIT_BEILV,

    EDIT_T_H,
    EDIT_L_ARM,
    EDIT_S_ARM,
    EDIT_SPEED,
    STATIC_LIJU, //11


};

CMainMenu::CMainMenu()
{
    m_msg_delay = 0;

    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
    TIniFile cfg("ctx2000.ini");

    m_show_up_angle  =  cfg.ReadBool("display","up_angle",false);
    m_show_speed     =  cfg.ReadBool("display","speed",   false);
    m_show_dg_height =  cfg.ReadBool("display","height",  false);
    m_show_max_weight = cfg.ReadBool("display","max_weight",  false);


    for (int i = 0; i < 2; i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtns[i],this);
    }

    //lbl_angle = new CStatic(&commctrls[0],this);
    //lbl_dist  = new CStatic(&commctrls[1],this);

    edt_angle           = new CEdit(&commctrls[0],this);
    edt_dist            = new CEdit(&commctrls[1],this);
    edt_max_weight      = new CEdit(&commctrls[2],this);
    edt_weight          = new CEdit(&commctrls[3],this);
    edt_height          = new CEdit(&commctrls[4],this);
    edt_up_angle        = new CEdit(&commctrls[5],this);
    edt_beilv           = new CEdit(&commctrls[6],this);

    edt_tower_height    = new CEdit(&commctrls[7],this);

    edt_long_arm_len    = new CEdit(&commctrls[8],this);
    edt_short_arm_len   = new CEdit(&commctrls[9],this);
    edt_fengsu          = new CEdit(&commctrls[10],this);
    m_liju              = new CStatic(&commctrls[11],this);

    /*
    if(m_show_up_angle)
    {
        new CStatic(&commctrls[12],this);
        edt_up_angle= new CEdit(&commctrls[8],this);
    }
    if(m_show_max_weight)
    {
        new CStatic(&commctrls[13],this);
        edt_max_weight = new CEdit(&commctrls[9],this);
    }
    if(m_show_speed)
    {
        new CStatic(&commctrls[14],this);
        edt_fengsu = new CEdit(&commctrls[10],this);

    }
    if(m_show_dg_height)
    {
        new CStatic(&commctrls[15],this);
        edt_dg_height = new CEdit(&commctrls[11],this);
    }
    lbl_max_weight = new CStatic(&commctrls[18],this);
    lbl_beilv     = new CStatic(&commctrls[17],this);


    lbl_rights[0] = new CStatic(&commctrls[5],this);
    lbl_rights[1] = new CStatic(&commctrls[6],this);
    lbl_rights[2] = new CStatic(&commctrls[7],this);
*/
    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
    assert(Font24 != NULL);
    Font24->setFontColor(RGB2Pixel(HDC_SCREEN,255,0,0));

    fprintf(stderr,"sbc=%s,mbc=%s\n",Font24->pfont->sbc_devfont->name,Font24->pfont->mbc_devfont->name);


    SetRect(&m_status_rect,    200,0,800,Client_Height);
    SetRect(&m_dev_serail_rect,0,0,200,  Client_Height);


    for(int i = 0; i < 1; i++)
    {
        m_areas[i] = new CStatic(&areactrls[i],this);
    }

    m_worksite = new CFormWorksite(m_areas[0],areactrls[0].w,areactrls[0].h);

    m_dir_mgr = new CDirStatusMgr(10,Client_Height+25);

    InitSkinHeader("MainMenu");
}

CMainMenu::~CMainMenu()
{
    if(m_worksite)
    {
        delete m_worksite;
    }
}
void CMainMenu::DrawDevSerial(HDC hdc, RECT rt,std::string devserail)
{
    SetPenColor(hdc,PIXEL_lightgray);
    Rectangle(hdc, rt.left, rt.top, rt.right ,rt.bottom);

    DrawMyText(hdc,Font24,&rt,devserail);

}
void CMainMenu::CreateInfoArea(HDC hdc)
{
    SetPenColor(hdc,PIXEL_lightgray);

    Rectangle(hdc, 0, Client_Height, 200 ,480);
}
void CMainMenu::CreateStatusArea(HDC hdc, RECT rt)
{
    SetPenColor(hdc,PIXEL_lightgray);

    Rectangle(hdc, rt.left, rt.top, rt.right ,rt.bottom);
    //FocusRect(hdc, rt.left, rt.top, rt.right ,rt.bottom);
    //m_status_rect = rt;

}
void CMainMenu::OnCreate()
{
    RECT rt;
    bool local=false;
    int  width = 50;
    int  span  = 5;
    for(int i = 0; i < 5 ;i++)
    {
        rt.left   = 530+i*width+i*span;
        rt.top    = 5;
        rt.right  = rt.left + width;
        rt.bottom = rt.top  + 30;
        if(i == 3)
            statusIcon[i] = new CStatusIcon(this,i+1,rt,true);
        else
            statusIcon[i] = new CStatusIcon(this,i+1,rt);
        //statusIcon[i] = new CStatusIcon(this,20+i*20,100,8);
    }
    for(int i = 0; i < 5 ;i++)
    {
        rt.left   = 530+i*width+i*span;
        rt.top    = 40;
        rt.right  = rt.left + width;
        rt.bottom = rt.top  + 30;
        if(i == 3)
            statusIcon[i+5] = new CStatusIcon(this,i+6,rt,true);
        else
            statusIcon[i+5] = new CStatusIcon(this,i+6,rt);
        //statusIcon[i] = new CStatusIcon(this,20+i*20,100,8);
    }

    SetTimer(m_hWnd,100,10);

}

int paintcircle(int x,int r)
{
    int y = (int)sqrt(r*r -x*x);
    //fprintf(stderr,"x=%d y=%d r=%d\n",x,y,r);
    return y;

}



void CMainMenu::OnShow()
{

    m_worksite->init(m_hWnd);
    m_per.Init(this,m_liju,commctrls[16].w,commctrls[16].h);
    edt_height->SetFloatText(g_TC[g_local_id].Height,1);
    edt_long_arm_len->SetFloatText(g_TC[g_local_id].LongArmLength,1);
    edt_short_arm_len->SetFloatText(g_TC[g_local_id].ShortArmLenght,1);

    m_hdc = GetDC(m_hWnd);

}
void CMainMenu::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);

    CreateStatusArea(hdc ,m_status_rect);
    DrawDevSerial(hdc,m_dev_serail_rect,"No.C8001");
    CreateInfoArea(hdc);

    for(int i = 0; i < 2 ;i++)
        statusIcon[i]->SetStatus(hdc,g_status[i]);
    for(int i = 2; i < 6 ;i++)
        statusIcon[i]->SetStatus(hdc,g_status[i]);
    for(int i = 6; i < 10 ;i++)
        statusIcon[i]->SetStatus(hdc,g_status[i]);

    EndPaint(hWnd, hdc);
}
void CMainMenu::OnTimer(int ID)
{


    /*lbl_dist->SetText(Poco::format("%0.2f",g_car_dist));
    //lbl_angle->SetText(Poco::format("%0.2f",g_angle));

    if(m_show_up_angle)
    {
        edt_up_angle->SetFloatText(g_up_angle,0);
    }
    if(m_show_speed)
    {
        edt_fengsu->SetFloatText(g_speed,0);
    }
    if(m_show_dg_height)
    {
        edt_dg_height->SetFloatText(g_dg_height,0);
    }
    if(m_show_max_weight)
    {
        edt_max_weight->SetFloatText(g_dg_weight,0);
    }*/
    m_per.Show(CLijuCtrl::Get().m_percent);
    m_worksite->update();
    //lbl_max_weight->SetText(Poco::format("%d",(int)CLijuCtrl::Get().m_max_weight));
//up down left right
/*
    if(CMainCtrl::Get().m_control_state.b1)
    {
        m_dir_mgr->Show(m_hdc,"right",EALARM);
    }else{
        m_dir_mgr->Show(m_hdc,"right",EOK);
    }
    if(CMainCtrl::Get().m_control_state.b3)
    {
        m_dir_mgr->Show(m_hdc,"left",EALARM);
    }else{
        m_dir_mgr->Show(m_hdc,"left",EOK);
    }

    if(CMainCtrl::Get().m_control_state.b3)
    {
        m_dir_mgr->Show(m_hdc,"left",EALARM);
    }else{
        m_dir_mgr->Show(m_hdc,"left",EOK);
    }

    if(CMainCtrl::Get().m_control_state.b9)
    {
        m_dir_mgr->Show(m_hdc,"left",EWARNING);
    }else{
        m_dir_mgr->Show(m_hdc,"left",EOK);
    }
    if(CMainCtrl::Get().m_control_state.b8)
    {
        m_dir_mgr->Show(m_hdc,"right",EWARNING);
    }else{
        m_dir_mgr->Show(m_hdc,"right",EOK);
    }
    if(CMainCtrl::Get().m_control_state.b4)
    {
        m_dir_mgr->Show(m_hdc,"up",EWARNING);
    }else{
        m_dir_mgr->Show(m_hdc,"up",EOK);
    }
    if(CMainCtrl::Get().m_control_state.b5)
    {
        m_dir_mgr->Show(m_hdc,"up",EWARNING);
    }else{
        m_dir_mgr->Show(m_hdc,"up",EOK);
    }
    if(CMainCtrl::Get().m_control_state.b6)
    {
        m_dir_mgr->Show(m_hdc,"down",EWARNING);
    }else{
        m_dir_mgr->Show(m_hdc,"down",EOK);
    }
    if(CMainCtrl::Get().m_control_state.b7)
    {
        m_dir_mgr->Show(m_hdc,"down",EWARNING);
    }else{
        m_dir_mgr->Show(m_hdc,"down",EOK);
    }
*/

}
void CMainMenu::OnButtonClick(skin_item_t* item)
{
    if(item->id == _skinBtns[0]->GetId()){
        CSysSet ss;
        ss.CreateForm(m_hWnd);
    }else if(item->id == _skinBtns[1]->GetId()){
        CLiJuManForm lj;
        lj.CreateForm(m_hWnd);

    }else if(item->id == _skinBtns[2]->GetId()){
        int ret = CLijuCtrl::Get().ChangeBeilv();
        //lbl_beilv->SetText(Poco::format("%d",ret));
        CLijuCtrl::Get().SaveData();
    }else if(item->id == _skinBtns[3]->GetId()){

    }else if(item->id == _skinBtns[4]->GetId()){

    }else if(item->id == _skinBtns[5]->GetId()){

    }

}
