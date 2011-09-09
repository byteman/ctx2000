// MainMenu.cpp: implementation of the CMainMenu class.
//
//////////////////////////////////////////////////////////////////////
#include "SingleDynForm.h"
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
#include "torque_mgr.h"
#include "mainctrl.h"
#include "SoftKeyboard.h"
#include <math.h>
#include <Poco/Format.h>
#include <Poco/LocalDateTime.h>
#include <Poco/DateTimeFormatter.h>
#include "Password.h"
#include "jdqadmin.h"
#include "MsgBox.h"
#define SKIN_BUTTON_MAINCFG     {1,22,410}
#define SKIN_BUTTON_BYPASS      {2,92,410}

#define E_W  85
#define E_H  30
#define L_S  55
#define L_S_V 110
#define V_S  47

#define L_S2   695
#define L_S_V2 195

#define EDIT_ANGLE    {60,280,  E_W,E_H,"0"}
#define EDIT_DIST     {315,305, 180,60,"0"}

#define EDIT_WEIGHT   {530, 225,E_W,E_H+10,"kg"}
#define EDIT_HEIGHT   {630, 305,150,E_H+10,"height"}
#define EDIT_UP_ANGL  {380, 170, 180,60,"upangle"}
#define EDIT_BEILV    {665, 125, E_W, 30,""}

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

CSingleDynForm::CSingleDynForm()
{
    m_msg_delay = 0;

    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }

    g_show_up_angle=true;

    for (int i = 0; i < 2; i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtns[i],this);
    }

#if 0
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
    if(g_show_max_weight){
        fast_max_weight      = new CFastStatic(&commctrls[2],this);
        fast_weight          = new CFastStatic(&commctrls[3],this);
    }
    fast_up_angle        = new CFastStatic(&commctrls[5],this);
    fast_beilv           = new CFastStatic(&commctrls[6],this);
    fast_percent         = new CFastStatic(&commctrls[12],this);
    m_fall_fact          = new DT_Percent(&commctrls[13],this);
    edt_time             = new CStatic(&commctrls[14],this);
    fast_time            = new CFastStatic(&commctrls[14],this);
    if(g_show_speed)
    {
        edt_fengsu          = new CStatic(&commctrls[10],    this);
        fast_fengsu         = new CFastStatic(&commctrls[10],this);
    }

    if(g_show_dg_height){
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
    gDispFilter[1].m_span = 0.01;
    InitSkinHeader("SingleTaji1");
}

CSingleDynForm::~CSingleDynForm()
{

}
void CSingleDynForm::OnCreate()
{
    SetTimer(m_hWnd,100,10);
}

void CSingleDynForm::OnShow()
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
    fast_time->Create ();
    fast_angle->Create();
    fast_dist->Create();

    fast_beilv->Create();
    fast_percent->Create();
    fast_up_angle->Create();
    if(g_show_max_weight){
        fast_weight->Create();
        fast_max_weight->Create();
    }
    if(g_show_speed)
    {
       fast_fengsu->Create();
    }
    if(g_show_dg_height){
       fast_height->Create();
    }
    fast_beilv->SetText(StrTCRate,color_black,Font24);

}
void CSingleDynForm::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);


    EndPaint(hWnd, hdc);
}
void CSingleDynForm::EmulateSensor()
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

void CSingleDynForm::OnTimer(int ID)
{
    //if(gDispFilter[disp_angle].need_update (g_angle))
    {
        fast_angle->SetText(Poco::format("%0.1f",g_angle),color_black,Font24);
    }
    //if(gDispFilter[disp_dist].need_update (g_car_dist))
    {
        fast_dist->SetText(Poco::format("%0.1fm",g_car_dist),color_black,Font40);
    }
    fast_beilv->SetText(StrTCRate,color_black,Font24);
    fast_up_angle->SetText(Poco::format("%0.1f",g_up_angle),color_black,Font40);
    if(g_show_max_weight)
    {
        //if(gDispFilter[disp_weight].need_update (g_dg_weight))
        {
            fast_weight->SetText(Poco::format("%0.1ft",g_dg_weight),color_black,Font40);
        }
        fast_max_weight->SetText(Poco::format("%0.1ft",CTorQueMgr::get ().m_rated_weight),color_black,Font40);
        double per = CTorQueMgr::get ().m_percent;
        //if(gDispFilter[disp_percent].need_update (per))
        {
            char buf[32] = {0,};
            int tmp = per*100.0f+0.5;
            snprintf(buf,32,"%d%%",tmp);
            fast_percent->SetText(buf,color_black,Font24);
        }

    }
    m_fall_fact->Show(CTorQueMgr::get ().m_percent);
    if(g_show_dg_height)
    {
        //if(gDispFilter[disp_height].need_update (g_dg_height))
        {
              fast_height->SetText(Poco::format("%0.1fm",g_dg_height),color_black,Font40);
        }

    }

    //fast_max_weight->SetText(Poco::format("%0.1ft",CTorQueMgr::get ().m_rated_weight),color_black,Font40);
    if(g_show_speed)
    {
        //if(gDispFilter[disp_speed].need_update (g_wild_speed))
        {
            fast_fengsu->SetText(Poco::format("F=%0.1fm/s",g_wild_speed),color_black,Font24);

        }
    }
    static int cnt = 0;
    if( (cnt%10) == 0)
    {
        Poco::LocalDateTime now;
        std::string dt = Poco::DateTimeFormatter::format(now,"%d/%m/%Y    %h:%M:%S    %w");

        std::string out = TCTypeName + "    " + dt;
        fast_time->SetText(out,color_black,Font24);
    }

}
void CSingleDynForm::OnLButtonUp(int x, int y)
{
    fprintf(stderr,"x=%d,y=%d\n",x,y);
    if(PtInRect(&m_liju_rect,x,y))
    {
        StrTCRate = Poco::format("%d",CTorQueMgr::get ().changeRate ());
        fast_beilv->SetText(StrTCRate,color_black,Font24);

        CTorQueMgr::get ().saveCfg ();
    }
}
void CSingleDynForm::OnButtonClick(skin_item_t* item)
{
    if(item->id == _skinBtns[0]->GetId()){


        PassWord psd;
        if(psd.ShowBox (this,"����:","ϵͳ��������","8334"))
        {
            KillTimer(m_hWnd,100);
            CSysSet ss;
            ss.CreateForm(m_hWnd);
            SetTimer(m_hWnd,100,10);
        }

    }else if(item->id == _skinBtns[1]->GetId()){
#if 1
        PassWord psd;
        if(psd.ShowBox (this,"����:","Bypass����","hitech"))
        {
            CJDQAdmin::Get ().Bypass (true);
            CMainCtrl::Get ().NotifyBypass (true);
            MsgBox box;
            box.ShowBox (this,"ȡ��bypass","Bypass����");
            CJDQAdmin::Get ().Bypass (false);
            CMainCtrl::Get ().NotifyBypass (false);
        }
#endif
        //CJDQAdmin::Get ().ResetDevice (1);
    }

}
