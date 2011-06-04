// MainMenu.cpp: implementation of the CMainMenu class.
//
//////////////////////////////////////////////////////////////////////
#include "SinalTaji2.h"
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

#define EDIT_ANGLE    {60,280,  E_W,E_H,"angle"}
#define EDIT_DIST     {310,310, 180,60,"dist"}

#define EDIT_WEIGHT   {520, 225,E_W,E_H,"kg"}
#define EDIT_HEIGHT   {590, 305,E_W,E_H,"height"}
#define EDIT_UP_ANGL  {390, 160, 180,60,"upangle"}
#define EDIT_BEILV    {630, 125, E_W, 30,"5"}

#define STATIC_LIJU    {375,55,  150,60,"liju"}
#define EDIT_MAX_W     {575,50,  150,80,"maxw"}

#define EDIT_T_H      {L_S2,L_S_V2+0*V_S, E_W,E_H,""}
#define EDIT_L_ARM    {L_S2,L_S_V2+1*V_S, E_W,E_H,""}
#define EDIT_S_ARM    {L_S2,L_S_V2+2*V_S, E_W,E_H,""}

#define EDIT_SPEED    {50,350, E_W,E_H,"1"}
#define EDIT_PERCENT  {385,50, E_W,E_H,"80%"}

#define Client_Height 72


//SoftKeyboard *skt=NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static const char* mmenu_bmps[] = {
    "ctx2000/single_dy.png",
    //PMAINCFG,
   // PBYPASS
};
static SKIN_BUTTON_DESC SkinBtns[] = {
    //SKIN_BUTTON_MAINCFG,
    //SKIN_BUTTON_BYPASS
};
static EDevStatus g_status[20];

static COMM_CTRL_DESC areactrls[] = {
    {200,  Client_Height+0 , 450,400,""},
};

static const char* ICons[] = {

};
static COMM_CTRL_DESC commctrls[] = {

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
    EDIT_PERCENT
};

CSingleTaji2::CSingleTaji2()
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
        //_skinBtns[i] = new CSkinButton(&SkinBtns[i],this);
    }
#if 1
    edt_angle           = new CStatic(&commctrls[0],this);

    edt_dist            = new CStatic(&commctrls[1],this);
    edt_max_weight      = new CStatic(&commctrls[2],this);
    edt_weight          = new CStatic(&commctrls[3],this);
    edt_height          = new CStatic(&commctrls[4],this);
    edt_up_angle        = new CStatic(&commctrls[5],this);
    edt_beilv           = new CStatic(&commctrls[6],this);

    //edt_tower_height    = new CStatic(&commctrls[7],this);

    //edt_long_arm_len    = new CStatic(&commctrls[8],this);
    //edt_short_arm_len   = new CStatic(&commctrls[9],this);

    edt_fengsu          = new CStatic(&commctrls[10],this);
    edt_percent         = new CStatic(&commctrls[12],this);
    //m_liju              = new CStatic(&commctrls[11],this);
#endif
    fast_angle           = new CFastStatic(&commctrls[0],this);

    fast_dist            = new CFastStatic(&commctrls[1],this);
    fast_max_weight      = new CFastStatic(&commctrls[2],this);
    fast_weight          = new CFastStatic(&commctrls[3],this);
    fast_height          = new CFastStatic(&commctrls[4],this);
    fast_up_angle        = new CFastStatic(&commctrls[5],this);
    fast_beilv           = new CFastStatic(&commctrls[6],this);

    fast_tower_height    = new CFastStatic(&commctrls[7],this);

    fast_long_arm_len    = new CFastStatic(&commctrls[8],this);
    fast_short_arm_len   = new CFastStatic(&commctrls[9],this);
    fast_fengsu          = new CFastStatic(&commctrls[10],this);

    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
    assert(Font24 != NULL);
    Font24->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));

    SetRect(&m_status_rect,    200,0,800,Client_Height);
    SetRect(&m_dev_serail_rect,0,0,200,  Client_Height);

    SetRect(& m_tc_type_rect,   660,Client_Height+65,800,  Client_Height+115);
    SetRect(&m_liju_rect, 145, 345, 180, 380);

    //m_dir_mgr = new CDirStatusMgr(700,Client_Height+5);

    InitSkinHeader("SingleTaji1");
}

CSingleTaji2::~CSingleTaji2()
{

}
void CSingleTaji2::OnCreate()
{

    RECT rt;
/*
    if(!skt)
    {
        skt = new SoftKeyboard();
        skt->T9_Show(false);
    }
    */
    SetTimer(m_hWnd,100,10);

}

void CSingleTaji2::OnShow()
{
    /*
    fast_angle->Attach(edt_angle);
    fast_dist->Attach(edt_dist);
    fast_weight->Attach(edt_weight);
    fast_beilv->Attach(edt_beilv);
    fast_tower_height->Attach(edt_tower_height);
    fast_long_arm_len->Attach(edt_long_arm_len);
    fast_short_arm_len->Attach(edt_short_arm_len);
    fast_height->Attach(edt_height);
    fast_up_angle->Attach(edt_up_angle);
    fast_max_weight->Attach(edt_max_weight);
    fast_fengsu->Attach(edt_fengsu);
*/
    //m_per.Init(this,m_liju,commctrls[11].w,commctrls[11].h);

    //edt_tower_height->SetText(Poco::format("%0.1f",g_TC[g_local_id].Height));
    //edt_long_arm_len->SetText(Poco::format("%0.1f",g_TC[g_local_id].LongArmLength));
    //edt_short_arm_len->SetText(Poco::format("%0.1f",g_TC[g_local_id].ShortArmLenght));

    m_hdc = GetDC(m_hWnd);
}
void CSingleTaji2::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);


    EndPaint(hWnd, hdc);
}
void CSingleTaji2::EmulateSensor()
{
static    double angle = 0;
static    double dist  = 0;
    angle += 3.1415/180;
    g_angle = angle;
    dist++;
    if(dist>=g_TC[g_local_id].LongArmLength)dist = 0;
    g_car_dist = dist;
    g_TC[g_local_id].Angle    = angle;
    g_TC[g_local_id].Position = dist;
}

void CSingleTaji2::OnTimer(int ID)
{
    //刷新角度
    edt_angle->SetText(Poco::format("%0.1f",g_angle));
    //刷新小车幅度
    edt_dist->SetText(Poco::format("%0.1f",g_car_dist));
    //是否显示额定重量和当前重量
    edt_max_weight->SetText(Poco::format("%d",(int)CLijuCtrl::Get().m_max_weight));
    edt_weight->SetText(Poco::format("%0.1f",g_dg_weight));
    edt_beilv->SetText(CLijuCtrl::Get().m_curBeilv);
    edt_height->SetText(Poco::format("%0.1f",g_dg_height));
    edt_up_angle->SetText(Poco::format("%0.1f",g_up_angle));
    edt_fengsu->SetText(Poco::format("%0.1f",g_speed));
    edt_percent->SetText(Poco::format("%0.1f",CLijuCtrl::Get().m_percent));


}
void CSingleTaji2::OnLButtonUp(int x, int y)
{
    fprintf(stderr,"x=%d,y=%d\n",x,y);
    if(PtInRect(&m_liju_rect,x,y))
    {
        int ret = CLijuCtrl::Get().ChangeBeilv();
        edt_beilv->SetText(Poco::format("%d",ret));
        CLijuCtrl::Get().SaveData();
    }
}
void CSingleTaji2::OnButtonClick(skin_item_t* item)
{
    if(item->id == _skinBtns[0]->GetId()){
        KillTimer(m_hWnd,100);
        CSysSet ss;
        ss.CreateForm(m_hWnd);
        SetTimer(m_hWnd,100,10);
    }
}
