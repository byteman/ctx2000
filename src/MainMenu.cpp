// MainMenu.cpp: implementation of the CMainMenu class.
//
//////////////////////////////////////////////////////////////////////
#include <math.h>
#include <Poco/Format.h>
#include <Poco/Random.h>
#include "MainMenuItem.h"
#include "MainMenu.h"
#include "statusicon.h"
#include "BmpRes.h"
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


#define E_W   82
#define E_H   27
#define L_S   52
#define L_S_V 108
#define V_S   47

#define L_S2   688
#define L_S_V2 192

/*
塔机绘制区域的高度位置 y pos
*/
#define Client_Height 72
/*
所有的文本框位置
*/
#define EDIT_ANGLE    {L_S,L_S_V+0*V_S, E_W,E_H,""}
#define EDIT_DIST     {L_S,L_S_V+1*V_S, E_W,E_H,""}
#define EDIT_MAX_W    {L_S,L_S_V+2*V_S, E_W,E_H,""}
#define EDIT_WEIGHT   {L_S,L_S_V+3*V_S, E_W,E_H,""}
#define EDIT_HEIGHT   {L_S,L_S_V+4*V_S, E_W,E_H,""}
#define EDIT_UP_ANGL  {L_S,L_S_V+5*V_S, E_W,E_H,""}
#define EDIT_FALL     {150,350,         30, 30, ""}
#define STATIC_LIJU_PERCENT    {142,80, 50,200, ""}

#define EDIT_T_H      {L_S2,L_S_V2+0*V_S, E_W+2,E_H,""}
#define EDIT_L_ARM    {L_S2,L_S_V2+1*V_S, E_W+2,E_H+2,""}
#define EDIT_S_ARM    {L_S2,L_S_V2+2*V_S, E_W+2,E_H,""}
#define EDIT_SPEED    {L_S2,L_S_V2+3*V_S+5, E_W+2,E_H,""}
static COMM_CTRL_DESC commctrls[] = {

    EDIT_ANGLE,
    EDIT_DIST,
    EDIT_MAX_W,
    EDIT_WEIGHT,
    EDIT_HEIGHT,
    EDIT_UP_ANGL,
    EDIT_FALL,

    EDIT_T_H,
    EDIT_L_ARM,
    EDIT_S_ARM,
    EDIT_SPEED, //speed
    STATIC_LIJU_PERCENT, //11
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static const char* mmenu_bmps[] = {
    PMAINBK,
    PMAINCFG,
    PBYPASS
};
static SKIN_BUTTON_DESC SkinBtns[] = {
    SKIN_BUTTON_MAINCFG,
    SKIN_BUTTON_BYPASS
};
static EDevStatus g_status[20];
/*
塔机绘制区域
*/
static COMM_CTRL_DESC areactrls[] = {
    {200,  Client_Height+0 , 450,400,""},
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
/*
软键盘输入法对象
*/
SoftKeyboard *skt=NULL;

CMainMenu::CMainMenu()
{
    m_msg_delay = 0;

    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
#if 1
    TIniFile cfg("ctx2000.ini");

    if(g_TC[g_local_id].Dyna)
        m_show_up_angle=true;
    else
        m_show_up_angle=false;

    m_show_speed      = cfg.ReadBool("display","speed",   false);
    m_show_dg_height  = cfg.ReadBool("display","height",  false);


    for (int i = 0; i < 2; i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtns[i],this);
    }

    edt_angle           = new CStatic(&commctrls[0],this);

    edt_dist            = new CStatic(&commctrls[1],this);
    edt_max_weight      = new CStatic(&commctrls[2],this);
    edt_weight          = new CStatic(&commctrls[3],this);


    edt_fall            = new CStatic(&commctrls[6],this);

    edt_tower_height    = new CStatic(&commctrls[7],this);

    edt_long_arm_len    = new CStatic(&commctrls[8],this);
    edt_short_arm_len   = new CStatic(&commctrls[9],this);
    m_liju              = new CStatic(&commctrls[11],this);


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

    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
    Font24->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));

    fprintf(stderr,"sbc=%s,mbc=%s\n",Font24->pfont->sbc_devfont->name,Font24->pfont->mbc_devfont->name);


    SetRect(&m_status_rect,     200,0,800,Client_Height);
    SetRect(&m_dev_serail_rect, 0,0,200,  Client_Height);
    SetRect(& m_tc_type_rect,   660,Client_Height+65,800,  Client_Height+115);
    SetRect(&m_fall_rect,       145, 345, 180, 380);
    SetRect(&m_quit_rect,       680,  413, 800,480);
    for(int i = 0; i < 1; i++)
    {
        m_areas[i] = new CStatic(&areactrls[i],this);
    }

    m_worksite = new CFormWorksite(m_areas[0],areactrls[0].w,areactrls[0].h);
    m_dir_mgr  = new CDirStatusMgr(700,Client_Height+5);
    m_lables.AddIcons(ICons,4);
#endif
    InitSkinHeader("MainMenu");
}

CMainMenu::~CMainMenu()
{
    if(m_worksite) delete m_worksite;
    if(m_worksite) delete m_worksite;
    if(m_dir_mgr)  delete m_dir_mgr;
}
/*
绘制塔机类型
*/
void CMainMenu::DrawTCType(HDC hdc, RECT rt,std::string tctype)
{
    SetPenColor(hdc,PIXEL_black);

    DrawMyText(hdc,Font24,&rt,tctype);
}
/*
绘制设备序列号
*/
void CMainMenu::DrawDevSerial(HDC hdc, RECT rt,std::string devserail)
{
    SetPenColor(hdc,PIXEL_lightgray);
    Rectangle(hdc, rt.left, rt.top, rt.right ,rt.bottom);

    DrawMyText(hdc,Font24,&rt,devserail);

}
/*
绘制一个左边的实时参数区域
*/
void CMainMenu::CreateInfoArea(HDC hdc)
{
    SetPenColor(hdc,PIXEL_lightgray);

    Rectangle(hdc, 0, Client_Height, 200 ,480);
}
/*
绘制一个右上的通讯状态区域
*/
void CMainMenu::CreateStatusArea(HDC hdc, RECT rt)
{
    SetPenColor(hdc,PIXEL_lightgray);

    Rectangle(hdc, rt.left, rt.top, rt.right ,rt.bottom);
}
void CMainMenu::OnCreate()
{

    RECT rt;
    int  width   = 30;
    int  height  = 20;
    int  span    = 5;
    int  START_X = 800-10*(width+span);

    for(int i = 0; i < 10 ;i++)
    {
        rt.left   = START_X + i*width+i*span;
        rt.top    = 5;
        rt.right  = rt.left + width;
        rt.bottom = rt.top  + height;
        if(i == 6)
            statusIcon[i] = new CStatusIcon(this,i+1,rt,true);
        else
            statusIcon[i] = new CStatusIcon(this,i+1,rt);
    }
    for(int i = 0; i < 10 ;i++)
    {
        rt.left   = START_X+i*width+i*span;
        rt.top    = 40;
        rt.right  = rt.left + width;
        rt.bottom = rt.top  + height;
        if(i == 6)
            statusIcon[i+10] = new CStatusIcon(this,i+11,rt,true);
        else
            statusIcon[i+10] = new CStatusIcon(this,i+11,rt);

    }

    if(!skt)
    {
        skt = new SoftKeyboard();
        skt->T9_Show(false);
    }
    SetTimer(m_hWnd,100,10);

}
void CMainMenu::UpdateUIArea()
{
    HDC hdc = GetClientDC(m_hWnd);

    CreateStatusArea(hdc ,m_status_rect);
    DrawDevSerial(hdc,m_dev_serail_rect,CurSerial);
    DrawTCType(hdc,m_tc_type_rect,TCTypeName);
    CreateInfoArea(hdc);
    for(int i = 0; i < 20 ; i++)
        statusIcon[i]->SetStatus(hdc,g_status[i]);

    ReleaseDC(hdc);
    //显示塔机高度
     fast_tower_height->SetText(Poco::format("%0.1fm",g_TC[g_local_id].Height));
    //显示大臂长度
     fast_long_arm_len->SetText(Poco::format("%0.1fm",g_TC[g_local_id].LongArmLength));
    //显示短臂长度
     fast_short_arm_len->SetText(Poco::format("%0.1fm",g_TC[g_local_id].ShortArmLenght));
     edt_fall->SetText(StrTCBeilv);
}
void CMainMenu::OnShow()
{
#if 1
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

    UpdateUIArea();

    m_worksite->init(m_hWnd);
    m_worksite->updateAll();
    m_per.Init(this,m_liju,commctrls[11].w,commctrls[11].h);

    m_hdc = GetDC(m_hWnd);
#endif
}
void CMainMenu::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);
#if 1
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
#endif
    EndPaint(hWnd, hdc);
}
void CMainMenu::EmulateSensor()
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

void CMainMenu::OnTimer(int ID)
{
    //EmulateSensor();
#if 1
    //刷新角度

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


    m_per.Show(CLijuCtrl::Get().m_percent);
    m_worksite->update();

    int status = CLijuCtrl::Get().m_cur_state;
    if(CMainCtrl::Get().m_control_state.b1)
    {
        m_dir_mgr->Show(m_hdc,"right",EALARM);
    }else{
        if(CMainCtrl::Get().m_control_state.b10)
        {
            m_dir_mgr->Show(m_hdc,"right",EWARNING);
        }else{
            m_dir_mgr->Show(m_hdc,"right",EOK);
        }
    }
    if(CMainCtrl::Get().m_control_state.b3)
    {
        m_dir_mgr->Show(m_hdc,"left",EALARM);
    }else{
        if(CMainCtrl::Get().m_control_state.b9)
        {
            m_dir_mgr->Show(m_hdc,"left",EWARNING);
        }else{
            m_dir_mgr->Show(m_hdc,"left",EOK);
        }
    }

    if(CMainCtrl::Get().m_control_state.b5)
    {
        m_dir_mgr->Show(m_hdc,"up",EALARM);
    }else{
        if(CMainCtrl::Get().m_control_state.b4)
        {
            m_dir_mgr->Show(m_hdc,"up",EWARNING);
        }else{
            m_dir_mgr->Show(m_hdc,"up",EOK);
        }
    }

    if(CMainCtrl::Get().m_control_state.b6)
    {
        m_dir_mgr->Show(m_hdc,"down",EALARM);
    }else{
        if(CMainCtrl::Get().m_control_state.b7)
        {
            m_dir_mgr->Show(m_hdc,"down",EWARNING);
        }else{
            m_dir_mgr->Show(m_hdc,"down",EOK);
        }
    }

#endif

}
void CMainMenu::OnLButtonUp(int x, int y)
{
    static int cnt=0;
    fprintf(stderr,"x=%d,y=%d\n",x,y);
    if(PtInRect(&m_fall_rect,x,y))
    {
        edt_fall->SetText(Poco::format("%d",CLijuCtrl::Get().ChangeBeilv()));
        CLijuCtrl::Get().SaveData();
    }else if(PtInRect(&m_quit_rect,x,y)){
        cnt++;
        if(cnt > 2){
            Close();
        }
    }else{
       cnt = 0;
    }
}
void CMainMenu::OnButtonClick(skin_item_t* item)
{
    if(item->id == _skinBtns[0]->GetId()){
        KillTimer(m_hWnd,100);
        CSysSet ss;
        ss.CreateForm(m_hWnd);
        SetTimer(m_hWnd,100,10);
        UpdateUIArea();
        m_worksite->updateAll();
    }else if(item->id == _skinBtns[1]->GetId()){

    }

}
