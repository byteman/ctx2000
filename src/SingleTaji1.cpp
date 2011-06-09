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
static const char* ICons[] = {
    "ctx2000/height.jpg",
    "ctx2000/up_angle.jpg",
    "ctx2000/speed.jpg",
    "ctx2000/edt_bk.jpg"
};

static POS IConsPos[] = {
    {18,300},           //height lable
    {L_S,L_S_V+4*V_S}, //height edit

    {18,343},           //up angle lable
    {L_S,L_S_V+5*V_S},//upangle edit

    {685,315},            //speed lable
    {L_S2-5,L_S_V2+3*V_S+3}//speed edit
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

    if(g_TC[g_local_id].Dyna)m_show_up_angle=true;

    m_show_speed      = cfg.ReadBool("display","speed",   false);
    m_show_dg_height  = cfg.ReadBool("display","height",  false);



    for (int i = 0; i < 2; i++)
    {
        //_skinBtns[i] = new CSkinButton(&SkinBtns[i],this);
    }

    edt_angle           = new CStatic(&commctrls[0],this);
    edt_dist            = new CStatic(&commctrls[1],this);
    edt_max_weight      = new CStatic(&commctrls[2],this);
    edt_weight          = new CStatic(&commctrls[3],this);
    edt_fall            = new CStatic(&commctrls[6],this);
    edt_percent         = new CStatic(&commctrls[12],this);
    //m_liju              = new CStatic(&commctrls[11],this);
#if 1
    fast_angle           = new CFastStatic(&commctrls[0],this);
    fast_dist            = new CFastStatic(&commctrls[1],this);
    fast_max_weight      = new CFastStatic(&commctrls[2],this);
    fast_weight          = new CFastStatic(&commctrls[3],this);
    fast_beilv           = new CFastStatic(&commctrls[6],this);
    fast_tower_height    = new CFastStatic(&commctrls[7],this);
    fast_long_arm_len    = new CFastStatic(&commctrls[8],this);
    fast_short_arm_len   = new CFastStatic(&commctrls[9],this);

    if(m_show_speed)
    {
        edt_fengsu          = new CStatic(&commctrls[10],    this);
        fast_fengsu         = new CFastStatic(&commctrls[10],this);
    }
    if( m_show_up_angle && m_show_dg_height)
    {
        edt_height          = new CStatic(&commctrls[4],this);
        edt_up_angle        = new CStatic(&commctrls[5],this);
        fast_height         = new CFastStatic(&commctrls[4],this);
        fast_up_angle       = new CFastStatic(&commctrls[5],this);
    }else if(m_show_up_angle){
        edt_up_angle        = new CStatic(&commctrls[4],this);
        fast_up_angle       = new CFastStatic(&commctrls[4],this);
    }else if(m_show_dg_height){
        edt_height          = new CStatic(&commctrls[4],this);
        fast_height         = new CFastStatic(&commctrls[4],this);
    }

#endif
    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
    assert(Font24 != NULL);
    Font24->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));

    SetRect(&m_status_rect,    200,0,800,Client_Height);
    SetRect(&m_dev_serail_rect,0,0,200,  Client_Height);

    SetRect(& m_tc_type_rect,   660,Client_Height+65,800,  Client_Height+115);
    SetRect(&m_liju_rect, 145, 345, 180, 380);

    edt_fall = new CStatic(&commctrls[11],this);

    m_lables.AddIcons(ICons,4);
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
    fast_angle->Attach(edt_angle);
    fast_dist->Attach(edt_dist);
    fast_weight->Attach(edt_weight);
    fast_beilv->Attach(edt_fall);
    fast_tower_height->Attach(edt_tower_height);
    fast_long_arm_len->Attach(edt_long_arm_len);
    fast_short_arm_len->Attach(edt_short_arm_len);


    fast_max_weight->Attach(edt_max_weight);
    if(m_show_dg_height)
        fast_height->Attach(edt_height);
    if(m_show_speed)
        fast_fengsu->Attach(edt_fengsu);
    if(m_show_up_angle)
        fast_up_angle->Attach(edt_up_angle);

}
void CSingleTaji1::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);

    if(m_show_dg_height)
    {
        m_lables.Show(hdc,IConsPos[0].x,IConsPos[0].y,1);
        m_lables.Show(hdc,IConsPos[1].x,IConsPos[1].y,4);
    }
    if(m_show_up_angle)
    {
        m_lables.Show(hdc,IConsPos[2].x,IConsPos[2].y,2);
        m_lables.Show(hdc,IConsPos[3].x,IConsPos[3].y,4);
    }
    if(m_show_speed)
    {
        m_lables.Show(hdc,IConsPos[4].x,IConsPos[4].y,3);
        m_lables.Show(hdc,IConsPos[5].x,IConsPos[5].y,4);
    }
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
    //EmulateSensor();
    fast_angle->SetText(Poco::format("%0.1f°",g_angle));
    //刷新小车幅度
    fast_dist->SetText(Poco::format("%0.1fm",g_car_dist));
    //是否显示额定重量和当前重量

    fast_max_weight->SetText(Poco::format("%0.1ft",CLijuCtrl::Get().m_max_weight));
    fast_weight->SetText(Poco::format("%0.1ft",g_dg_weight));

    //是否显示吊钩高度
    if(m_show_dg_height)
    {
       fast_height->SetText(Poco::format("%0.1fm",g_dg_height));
    }
    //是否显示仰角
    if(m_show_up_angle)
    {
        fast_up_angle->SetText(Poco::format("%0.1f°",g_up_angle));
    }
    //是否显示风速
    if(m_show_speed)
    {
        fast_fengsu->SetText(Poco::format("%0.1fm/s",g_speed));
    }
}
void CSingleTaji1::OnLButtonUp(int x, int y)
{

}
void CSingleTaji1::OnButtonClick(skin_item_t* item)
{

}
