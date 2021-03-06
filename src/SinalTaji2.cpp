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
#include <Poco/LocalDateTime.h>
#include <Poco/DateTimeFormatter.h>

#define SKIN_BUTTON_MAINCFG     {1,22,410}
#define SKIN_BUTTON_BYPASS      {2,92,410}

#define E_W  85
#define E_H  30
#define L_S  55
#define L_S_V 110
#define V_S  47

#define L_S2   695
#define L_S_V2 195

#define EDIT_ANGLE    {60,280,  E_W,E_H,"angle"}
#define EDIT_DIST     {315,305, 180,60,"dist"}

#define EDIT_WEIGHT   {530, 225,E_W,E_H+10,"kg"}
#define EDIT_HEIGHT   {630, 305,150,E_H+10,"height"}
#define EDIT_UP_ANGL  {380, 170, 180,60,"upangle"}
#define EDIT_BEILV    {665, 125, E_W, 30,"5"}

#define STATIC_LIJU    {375,55,  150,60,"liju"}
#define EDIT_MAX_W     {575,40,  150,E_H+10,"maxw"}

#define EDIT_T_H      {L_S2,L_S_V2+0*V_S, E_W,E_H,""}
#define EDIT_L_ARM    {L_S2,L_S_V2+1*V_S, E_W,E_H,""}
#define EDIT_S_ARM    {L_S2,L_S_V2+2*V_S, E_W,E_H,""}

#define EDIT_SPEED    {45,350, E_W+15,E_H,"1"}
#define EDIT_PERCENT  {385,55, E_W,E_H,"80%"}

#define Client_Height 72


extern SoftKeyboard *skt;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static const char* mmenu_bmps[] = {
    "ctx2000/single_dy.png",
     PMAINCFG,
     PBYPASS
};
static SKIN_BUTTON_DESC SkinBtns[] = {
    SKIN_BUTTON_MAINCFG,
    SKIN_BUTTON_BYPASS
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
    EDIT_PERCENT,
    {65,55,344-65,75-60,"10"},
    {200,410,697-144,423-392,"20"},

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
    m_show_up_angle=true;

    m_show_speed      = cfg.ReadBool("display","speed",   false);
    m_show_dg_height  = cfg.ReadBool("display","height",  false);

    for (int i = 0; i < 2; i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtns[i],this);
    }

#if 1
    edt_angle           = new CStatic(&commctrls[0],this);
    edt_dist            = new CStatic(&commctrls[1],this);
    edt_max_weight      = new CStatic(&commctrls[2],this);
    edt_weight          = new CStatic(&commctrls[3],this);
    edt_up_angle        = new CStatic(&commctrls[5],this);
    edt_beilv           = new CStatic(&commctrls[6],this);

    edt_percent         = new CStatic(&commctrls[12],this);

#endif
    fast_angle           = new CFastStatic(&commctrls[0],this);
    fast_dist            = new CFastStatic(&commctrls[1],this);
    fast_max_weight      = new CFastStatic(&commctrls[2],this);
    fast_weight          = new CFastStatic(&commctrls[3],this);
    fast_up_angle        = new CFastStatic(&commctrls[5],this);
    fast_beilv           = new CFastStatic(&commctrls[6],this);
    fast_percent         = new CFastStatic(&commctrls[12],this);
    m_fall_fact          = new DT_Percent(&commctrls[13],this);
    edt_time             = new CStatic(&commctrls[14],this);
    fast_time            = new CFastStatic(&commctrls[14],this);
    if(m_show_speed)
    {
        edt_fengsu          = new CStatic(&commctrls[10],    this);
        fast_fengsu         = new CFastStatic(&commctrls[10],this);
    }

    if(m_show_dg_height){
        edt_height          = new CStatic(&commctrls[4],this);
        fast_height         = new CFastStatic(&commctrls[4],this);
    }

    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
    Font40 = CFontMgr::Get().GetFont("stxinwei",40);

    color_black = RGB2Pixel(HDC_SCREEN,0,0,0);
    Font24->setFontColor(color_black);
    Font40->setFontColor(color_black);

    SetRect(&m_status_rect,    200,0,800,Client_Height);
    SetRect(&m_dev_serail_rect,0,0,200,  Client_Height);

    SetRect(& m_tc_type_rect,   660,Client_Height+65,800,  Client_Height+115);
    SetRect(&m_liju_rect, 617, 117, 700, 155);

    color_black  = RGB2Pixel(HDC_SCREEN,255,255,255);
    InitSkinHeader("SingleTaji1");
}

CSingleTaji2::~CSingleTaji2()
{

}
void CSingleTaji2::OnCreate()
{

    if(!skt)
    {
        skt = new SoftKeyboard();
        skt->T9_Show(false);
    }

    SetTimer(m_hWnd,100,10);

}

void CSingleTaji2::OnShow()
{

    m_fall_fact->Create();
    TColorSet rst;
    TColorPercent color;

    color.value = 0;
    color.color = PIXEL_green;
    rst.push_back(color);

    color.value = 0.9;
    color.color = PIXEL_yellow;
    rst.push_back(color);

    color.value = 1;
    color.color = PIXEL_red;
    rst.push_back(color);

    m_fall_fact->SetColorPercent(rst);
    fast_time->Attach(edt_time);
    fast_angle->Attach(edt_angle);
    fast_dist->Attach(edt_dist);
    fast_weight->Attach(edt_weight);
    fast_beilv->Attach(edt_beilv);
    fast_percent->Attach(edt_percent);
    fast_max_weight->Attach(edt_max_weight);
    fast_up_angle->Attach(edt_up_angle);
    if(m_show_speed)
    {
       fast_fengsu->Attach(edt_fengsu);
    }
    if(m_show_dg_height){
       fast_height->Attach(edt_height);
    }
    fast_beilv->SetText(StrTCBeilv,color_black,Font24);

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
    fast_angle->SetText(Poco::format("%0.1f°",g_angle),color_black,Font24);
    //刷新小车幅度
    fast_dist->SetText(Poco::format("%0.1fm",g_car_dist),color_black,Font40);

    //是否显示额定重量和当前重量
    fast_up_angle->SetText(Poco::format("%0.1f°",g_up_angle),color_black,Font40);
    fast_max_weight->SetText(Poco::format("%0.1ft",CLijuCtrl::Get().m_max_weight),color_black,Font40);
    fast_weight->SetText(Poco::format("%0.1ft",g_dg_weight),color_black,Font40);
    char buf[32] = {0,};
    int tmp = CLijuCtrl::Get().m_percent*100.0f+0.5;
    snprintf(buf,32,"%d%",tmp);
    //fprintf(stderr,"len = %d\n",tmp);
    buf[strlen(buf)]='%';
    m_fall_fact->Show(CLijuCtrl::Get().m_percent);

    fast_percent->SetText(buf,color_black,Font24);
    //是否显示吊钩高度

    if(m_show_dg_height)
    {
       fast_height->SetText(Poco::format("%0.1fm",g_dg_height),color_black,Font40);
    }


    fast_max_weight->SetText(Poco::format("%0.1ft",CLijuCtrl::Get().m_max_weight),color_black,Font40);
    if(m_show_speed)
        fast_fengsu->SetText(Poco::format("F=%0.1fm/s",g_speed),color_black,Font24);
    static int cnt = 0;
    if( (cnt%10) == 0)
    {
        Poco::LocalDateTime now;
        std::string dt = Poco::DateTimeFormatter::format(now,"%d/%m/%Y    %h:%M:%S    %w");

        std::string out = TCTypeName + "    " + dt;
        fast_time->SetText(out,color_black,Font24);
    }

}
void CSingleTaji2::OnLButtonUp(int x, int y)
{
    fprintf(stderr,"x=%d,y=%d\n",x,y);
    if(PtInRect(&m_liju_rect,x,y))
    {
        StrTCBeilv = Poco::format("%d",CLijuCtrl::Get().ChangeBeilv());
        fast_beilv->SetText(StrTCBeilv,color_black,Font24);

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
    }else if(item->id == _skinBtns[1]->GetId()){
        Close();
    }
}
