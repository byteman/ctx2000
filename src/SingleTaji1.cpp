// MainMenu.cpp: implementation of the CMainMenu class.
//
//////////////////////////////////////////////////////////////////////
#include "SingleTaji1.h"
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

#define STATIC_LIJU    {0,0,  200,200,"liju"}

#define EDIT_MAX_W     {100,150,  200,200,"maxw"}

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
    "ctx2000/mainbk.png",
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

CSingleTaji1::CSingleTaji1()
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
#if 0
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
#endif
    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
    assert(Font24 != NULL);
    Font24->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));

    SetRect(&m_status_rect,    200,0,800,Client_Height);
    SetRect(&m_dev_serail_rect,0,0,200,  Client_Height);

    SetRect(& m_tc_type_rect,   660,Client_Height+65,800,  Client_Height+115);
    SetRect(&m_liju_rect, 145, 345, 180, 380);

    edt_beilv = new CStatic(&commctrls[11],this);
    tj_img[0]    = new CTajiImage(&commctrls[11],this);
    tj_img[1]   = new CTajiImage(&commctrls[2],this);


    //m_dir_mgr = new CDirStatusMgr(700,Client_Height+5);

    InitSkinHeader("SingleTaji1");
}

CSingleTaji1::~CSingleTaji1()
{

}
void CSingleTaji1::OnCreate()
{
    SetTimer(m_hWnd,100,1);

}

void CSingleTaji1::OnShow()
{
    tj_img[0]->Attach(edt_beilv);
    tj_img[1]->Attach(edt_max_weight);
}
void CSingleTaji1::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);


    EndPaint(hWnd, hdc);
}
void CSingleTaji1::EmulateSensor()
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

void CSingleTaji1::OnTimer(int ID)
{
    EmulateSensor();
    tj_img[0]->UpdateInfo(g_angle,g_car_dist);
    tj_img[1]->UpdateInfo(g_angle,g_car_dist);

}
void CSingleTaji1::OnLButtonUp(int x, int y)
{

}
void CSingleTaji1::OnButtonClick(skin_item_t* item)
{

}
