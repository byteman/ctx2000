// MainMenu.cpp: implementation of the CMainMenu class.
//
//////////////////////////////////////////////////////////////////////
#include "SingleFlatForm.h"
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
#include "BmpRes.h"
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

#define EDIT_ANGLE    {60, 242, 100,60,"angle"}
#define EDIT_DIST     {290,285, 180,60,"dist"}

#define EDIT_WEIGHT   {417, 225,150,E_H+12,"t"}
#define EDIT_HEIGHT   {530, 284,150,E_H+12,""}
#define EDIT_UP_ANGL  {390, 160, 180,60,"upangle"}
#define EDIT_BEILV    {510, 145, E_W, 30,""}
#define STATIC_LIJU   {0,0, 200,200,"liju"}
#define EDIT_MAX_W    {566, 40,  150,40,"maxw"}
#define EDIT_T_H      {L_S2,L_S_V2+0*V_S, E_W,E_H,""}
#define EDIT_L_ARM    {L_S2,L_S_V2+1*V_S, E_W,E_H,""}
#define EDIT_S_ARM    {L_S2,L_S_V2+2*V_S, E_W,E_H,""}

#define EDIT_SPEED    {75,340, 80,25,""}
#define EDIT_PERCENT  {385,55, 120,E_H,"80%"}
#define Client_Height 72


extern SoftKeyboard *skt;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static const char* mmenu_bmps[] = {
    "ctx2000/pingbibk.png",
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
    {93,65,355-91,70-55,"10"},
    {186,358,472-186,366-348,"20"},
    {710,193,732-710,390-187,"20"},
    {200,410,697-144,423-392,"20"},
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
CSingleFlatForm::CSingleFlatForm()
{
    m_msg_delay = 0;

    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }

    if(g_TC[g_local_id].Dyna)g_show_up_angle=true;

    for (int i = 0; i < 2; i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtns[i],this);
    }

    edt_angle           = new CStatic(&commctrls[0],this);
    edt_dist            = new CStatic(&commctrls[1],this);
    edt_max_weight      = new CStatic(&commctrls[2],this);
    edt_weight          = new CStatic(&commctrls[3],this);
    edt_fall            = new CStatic(&commctrls[6],this);

    edt_percent         = new CStatic(&commctrls[12],this);
    edt_time            = new CStatic(&commctrls[16],this);
    fast_time           = new CFastStatic(&commctrls[16],this);
#if 1
    fast_angle           = new CFastStatic(&commctrls[0],this);
    fast_dist            = new CFastStatic(&commctrls[1],this);
    fast_beilv           = new CFastStatic(&commctrls[6],this);

    fast_percent         = new CFastStatic(&commctrls[12],this);
    if(g_show_speed)
    {
        edt_fengsu          = new CStatic(&commctrls[10],    this);
        fast_fengsu         = new CFastStatic(&commctrls[10],this);
    }

    if(g_show_dg_height){
        edt_height          = new CStatic(&commctrls[4],this);
        fast_height         = new CFastStatic(&commctrls[4],this);
    }

    if(g_show_max_weight){
        fast_max_weight      = new CFastStatic(&commctrls[2],this);
        fast_weight          = new CFastStatic(&commctrls[3],this);
    }
    m_fall_fact             = new DT_Percent(&commctrls[13],this);
    m_dist_fact             = new DT_Percent(&commctrls[14],this);
    m_hi_fact               = new DT_Percent(&commctrls[15],this,1);
#endif
    Font40 = CFontMgr::Get().GetFont("stxinwei",40);
    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
    fprintf(stderr,"===========sbc=%s,mbc=%s\n",Font24->pfont->sbc_devfont->name,Font24->pfont->mbc_devfont->name);


    Font40->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));
    Font24->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));

    SetRect(&m_status_rect,    200,0,800,Client_Height);
    SetRect(&m_dev_serail_rect,0,0,200,  Client_Height);

    SetRect(& m_tc_type_rect,   660,Client_Height+65,800,  Client_Height+115);
    SetRect(&m_fall_rect,       500, 142, 548, 175);

    m_lables.AddIcons(ICons,4);
    color_black = RGB2Pixel(HDC_SCREEN,255,255,255);
    InitSkinHeader("SingleTaji1");

}

CSingleFlatForm::~CSingleFlatForm()
{

}
void CSingleFlatForm::OnCreate()
{
    SetTimer(m_hWnd,100,10);
}

void CSingleFlatForm::OnShow()
{

    m_dist_fact->Create();
    m_fall_fact->Create();
    m_hi_fact->Create();
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

    fast_angle->Attach(edt_angle);
    fast_dist->Attach(edt_dist);

    fast_beilv->Attach(edt_fall);
    fast_percent->Attach(edt_percent);

    fast_time->Attach(edt_time);
    if(g_show_max_weight){
        fast_weight->Attach(edt_weight);
        fast_max_weight->Attach(edt_max_weight);
    }
    if(g_show_speed)
    {
       fast_fengsu->Attach(edt_fengsu);
    }
    if(g_show_dg_height){
       fast_height->Attach(edt_height);
    }
    fast_beilv->SetText(StrTCRate,color_black,Font24);


}
void CSingleFlatForm::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);

    EndPaint(hWnd, hdc);
}
void CSingleFlatForm::EmulateSensor()
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

void CSingleFlatForm::OnTimer(int ID)
{
    //EmulateSensor();

    fast_angle->SetText(Poco::format("%0.1f",g_angle),color_black,Font24);

    fast_dist->SetText(Poco::format("%0.1fm",g_car_dist),color_black,Font40);
    m_dist_fact->Show(g_car_dist/g_TC[g_local_id].LongArmLength);
    if(g_show_dg_height)
        m_hi_fact->Show(g_dg_height/g_TC[g_local_id].Height);
     if(g_show_max_weight){
         fast_max_weight->SetText(Poco::format("%0.1ft",CTorQueMgr::get ().m_rated_weight),color_black,Font40);
         fast_weight->SetText(Poco::format("%0.1ft",g_dg_weight),color_black,Font40);
         char buf[32] = {0,};
         int tmp = CTorQueMgr::get ().m_percent*100.0f+0.5;
         snprintf(buf,32,"%d%%",tmp);
         fast_percent->SetText(buf,color_black,Font24);
     }



    m_fall_fact->Show(CTorQueMgr::get ().m_percent);
    fast_beilv->SetText(StrTCRate,color_black,Font24);

    if(g_show_dg_height)
    {
       fast_height->SetText(Poco::format("%0.1fm",g_dg_height),color_black,Font40);
    }


    fast_max_weight->SetText(Poco::format("%0.1ft",CTorQueMgr::get ().m_rated_weight),color_black,Font40);
    if(g_show_speed)
        fast_fengsu->SetText(Poco::format("%0.1fm/s",g_wild_speed),color_black,Font24);

    static int cnt = 0;
    if( (cnt%10) == 0)
    {
        Poco::LocalDateTime now;
        std::string dt = Poco::DateTimeFormatter::format(now,"%d/%m/%Y    %h:%M    %w");

        std::string out = TCTypeName + "    " + dt;
        fast_time->SetText(out,color_black,Font24);
    }

}

void CSingleFlatForm::OnButtonClick(skin_item_t* item)
{
    if(item->id == _skinBtns[0]->GetId()){


        PassWord psd;
        if(psd.ShowBox (this,"ÃÜÂë:","ÏµÍ³ÉèÖÃÃÜÂë","8334"))
        {
            KillTimer(m_hWnd,100);
            CSysSet ss;
            ss.CreateForm(m_hWnd);
            SetTimer(m_hWnd,100,10);
        }

    }else if(item->id == _skinBtns[1]->GetId()){
#if 1
        PassWord psd;
        if(psd.ShowBox (this,"ÃÜÂë:","BypassÃÜÂë","hitech"))
        {
            CJDQAdmin::Get ().Bypass (true);
            CMainCtrl::Get ().NotifyBypass (true);
            MsgBox box;
            box.ShowBox (this,"È¡Ïûbypass","BypassÉèÖÃ");
            CJDQAdmin::Get ().Bypass (false);
            CMainCtrl::Get ().NotifyBypass (false);
        }
#endif
        //CJDQAdmin::Get ().ResetDevice (1);
    }
}
void CSingleFlatForm::OnLButtonUp(int x, int y)
{
    static int cnt=0;
    fprintf(stderr,"x=%d,y=%d\n",x,y);
    if(PtInRect(&m_fall_rect,x,y))
    {
        StrTCRate = Poco::format("%d",CTorQueMgr::get ().changeRate ());
        fast_beilv->SetText(StrTCRate,color_black,Font24);
        CTorQueMgr::get ().saveCfg ();
    }
}
