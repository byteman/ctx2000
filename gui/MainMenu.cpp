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
#include "torque_mgr.h"
#include "mainctrl.h"
#include "SoftKeyboard.h"
#include "guinotifyer.h"
#include "Password.h"
#include "jdqadmin.h"
#include "MsgBox.h"

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
#define EDIT_WEIGHT   {L_S,L_S_V+2*V_S, E_W,E_H,""}
#define EDIT_SPEED    {L_S,L_S_V+3*V_S, E_W,E_H,""}
#define EDIT_HEIGHT   {L_S,L_S_V+4*V_S, E_W,E_H,""}
#define EDIT_UP_ANGL  {L_S,L_S_V+5*V_S, E_W,E_H,""}


#define EDIT_FALL               {150,350,30, 30, ""}
#define STATIC_FALL_PERCENT     {142,80, 50,200, ""}
#define EDIT_T_H                {L_S2,L_S_V2+0*V_S, E_W+2,E_H,""}
#define EDIT_L_ARM              {L_S2,L_S_V2+1*V_S, E_W+2,E_H+2,""}
#define EDIT_S_ARM              {L_S2,L_S_V2+2*V_S, E_W+2,E_H,""}
#define EDIT_MAX_W              {L_S2-3,L_S_V2+3*V_S+15, E_W+2,E_H,""}
#define EDIT_MSG1               {300, 5,   150, 25,""} //7
#define EDIT_MSG2               {205, 35,  243, 32,""} //8


#define OPTION_POS_START   0

static COMM_CTRL_DESC option_comm_ctrls[] = {
    //=============
    EDIT_WEIGHT,    //0
    EDIT_SPEED,     //1
    EDIT_HEIGHT,    //2
    EDIT_UP_ANGL,   //3

};


static COMM_CTRL_DESC commctrls[] = {

    EDIT_ANGLE,  //0
    EDIT_DIST,   //1
    EDIT_FALL,  //2
    EDIT_T_H,   //3  tower height
    EDIT_L_ARM, //4  long  arm length
    EDIT_S_ARM, //5  short arm length
    STATIC_FALL_PERCENT, //6
    EDIT_MSG1,  //7
    EDIT_MSG2,  //8
    EDIT_MAX_W, //9
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
    {200,  Client_Height+0 , 470,410,""},
};

static const char* ICons[] = {
    "ctx2000/weight.png",
    "ctx2000/max_weight.png",
    "ctx2000/speed.jpg",
    "ctx2000/height.jpg",
    "ctx2000/up_angle.jpg",
    "ctx2000/edt_bk.jpg",
};

static POS IConsPos[] = {

    {12,198},            //option 1 lable
    {L_S,L_S_V+2*V_S},  //option  1 edit

    {18,252},            //option 2 lable
    {L_S,L_S_V+3*V_S},  //option 2 edit

    {20,300},           //option 3 lable
    {L_S,L_S_V+4*V_S},  //option 3 edit

    {18,343},           //option 4 lable
    {L_S,L_S_V+5*V_S},  //option 4 edit

    {680,315},            //max weight lable
    {L_S2-5,L_S_V2+3*V_S+12}//max weight edit
};
/*
软键盘输入法对象
*/
SoftKeyboard *skt=NULL;
void CMainMenu::LoadOptionItem()
{
    int pos = OPTION_POS_START;

    if(g_show_max_weight){
        fast_weight    = new CFastStatic(&option_comm_ctrls[pos++],this);
        fast_max_weight= new CFastStatic(&commctrls[9],this);
    }
    //fprintf(stderr,"show speed=%d\n",g_show_speed);
    if(g_show_speed)
    {

        fast_fengsu         = new CFastStatic(&option_comm_ctrls[pos++],this);
    }
    //fprintf(stderr,"show g_show_dg_height=%d\n",g_show_dg_height);
    if(g_show_dg_height){

        fast_height         = new CFastStatic(&option_comm_ctrls[pos++],this);
    }
    //fprintf(stderr,"show g_show_up_angle=%d\n",g_show_up_angle);
    if(g_show_up_angle){

        fast_up_angle       = new CFastStatic(&option_comm_ctrls[pos++],this);
    }
}
CMainMenu::CMainMenu()
{
    m_msg_delay     = 0;
    m_quit          = false;
    m_cur_signal_db = 0xff;

    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
#if 1

    if(g_TC[g_local_id].Dyna)
        g_show_up_angle=true;
    else
        g_show_up_angle=false;

    for (int i = 0; i < TABLESIZE(SkinBtns); i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtns[i],this);
    }
    fast_angle           = new CFastStatic(&commctrls[0],this);
    fast_dist            = new CFastStatic(&commctrls[1],this);

    LoadOptionItem();

    fast_fall            = new CFastStatic(&commctrls[2],this);
    fast_tower_height    = new CFastStatic(&commctrls[3],this);
    fast_long_arm_len    = new CFastStatic(&commctrls[4],this);
    fast_short_arm_len   = new CFastStatic(&commctrls[5],this);

    m_fall               = new CStatic(&commctrls[6],this);

    for(int j = 0; j < 2; j++)
    {
        fast_msg[j] = new CFastStatic(&commctrls[7+j],this);
    }

    Font16 = CFontMgr::Get().GetFont("song",16);
    Font16->setFontColor(PIXEL_black);
    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
    Font24->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));
    Font32 = CFontMgr::Get().GetFont("stxinwei",32);
    Font32->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));
    Font40= CFontMgr::Get().GetFont("stxinwei",40);
    Font40->setFontColor(RGB2Pixel(HDC_SCREEN,0,0,0));

    //fprintf(stderr,"sbc=%s,mbc=%s\n",Font24->pfont->sbc_devfont->name,Font24->pfont->mbc_devfont->name);


    SetRect(&m_status_rect,     200,0,800,Client_Height);
    SetRect(&m_dev_serail_rect, 0,415,200,  400+Client_Height);
    SetRect(&m_tc_type_rect,    0,0,200,  Client_Height);
    SetRect(&m_fall_rect,       145, 345, 180, 380);
    SetRect(&m_quit_rect,       680, 413, 800, 480);
    SetRect(&m_signal_rect,     210, 5,   210+26,5+26);

    for(int i = 0; i < 1; i++)
    {
        m_areas[i] = new CStatic(&areactrls[i],this);
    }

    m_worksite = new CFormWorksite(m_areas[0],areactrls[0].w,areactrls[0].h);
    m_dir_mgr  = new CDirStatusMgr(675,Client_Height+5);
    m_lables.AddIcons(ICons,TABLESIZE(ICons));
    m_signal.LoadFile("ctx2000/signal.png",6);
    m_cur_signal_db = 1;
#endif

    InitSkinHeader("MainMenu");
}

CMainMenu::~CMainMenu()
{
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

void CMainMenu::OnShow()
{
    fast_angle->Create();
    fast_dist->Create();
    fast_fall->Create();
    fast_tower_height->Create();
    fast_long_arm_len->Create();
    fast_short_arm_len->Create();
    if(g_show_max_weight){
        fast_weight->Create();
        fast_max_weight->Create();
    }
    if(g_show_dg_height)
        fast_height->Create();
    if(g_show_speed)
        fast_fengsu->Create();
    if(g_show_up_angle)
        fast_up_angle->Create();

    for(int j = 0; j < 2; j++)
    {
        fast_msg[j]->Create();
    }
    UpdateUIArea();
    m_worksite->init(m_hWnd);
    m_worksite->updateAll();
    m_per.Init(this,m_fall,commctrls[6].w,commctrls[6].h);
    TGuiNotifyer::Get().RegisterWindow(m_hWnd,1);
    TGuiNotifyer::Get().RegisterWindow(m_hWnd,2);
}
void CMainMenu::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);
    int start = OPTION_POS_START;
#if 1
    if(g_show_max_weight)
    {
        //weight
        m_lables.Show(hdc,IConsPos[start].x,IConsPos[start].y,1);
        m_lables.Show(hdc,IConsPos[start+1].x,IConsPos[start+1].y,6);
        //max weight
        m_lables.Show(hdc,IConsPos[8].x,IConsPos[8].y,2);
        m_lables.Show(hdc,IConsPos[9].x,IConsPos[9].y,6);
        start+=2;
    }

    if(g_show_speed)
    {
        //speed
        m_lables.Show(hdc,IConsPos[start].x,IConsPos[start].y,3);
        m_lables.Show(hdc,IConsPos[start+1].x,IConsPos[start+1].y,6);
        start+=2;
    }
    if(g_show_dg_height)
    {
        m_lables.Show(hdc,IConsPos[start].x,IConsPos[start].y,4);
        m_lables.Show(hdc,IConsPos[start+1].x,IConsPos[start+1].y,6);
        start+=2;
    }
    if(g_show_up_angle)
    {
        m_lables.Show(hdc,IConsPos[start].x,IConsPos[start].y,5);
        m_lables.Show(hdc,IConsPos[start+1].x,IConsPos[start+1].y,6);
        start+=2;
    }

    if( (m_cur_signal_db > 0 ) && (m_cur_signal_db < 7))
        m_signal.Show(hdc,210,5,m_cur_signal_db);

    for(int i = 0; i < 20 ; i++)
    {
        //printf("valid=%s\n",g_TC[i+1].Valide?"true":"false");
        statusIcon[i]->SetOnline (hdc,g_TC[i+1].Valide);
    }

#endif
    EndPaint(hWnd, hdc);
}

void CMainMenu::OnTimer(int ID)
{
    //EmulateSensor();
#if 1
    UpdateRealTimeParam();
    UpdateDevMode();
    UpdateCollideStatus();
    UpdateSignal();
#endif
    if(m_quit){
        KillTimer(m_hWnd,100);
        Close();
    }
}
void CMainMenu::OnLButtonUp(int x, int y)
{
    static int cnt=0;
    fprintf(stderr,"x=%d,y=%d\n",x,y);
    if(PtInRect(&m_fall_rect,x,y))
    {
        StrTCRate = Poco::format("%d",CTorQueMgr::get ().changeRate ());
        fast_fall->SetText(StrTCRate);
        CTorQueMgr::get ().saveCfg ();
    }else if(PtInRect(&m_quit_rect,x,y)){
        cnt++;
        if(cnt > 2){
            m_quit = true;
        }
    }else{
       cnt = 0;
    }
}
#include "CaliBox.h"
void CMainMenu::OnButtonClick(skin_item_t* item)
{
    if(item->id == _skinBtns[0]->GetId()){


        PassWord psd;
        if(psd.ShowBox (this,"密码:","系统设置密码","8334"))
        {
            KillTimer(m_hWnd,100);
            CSysSet ss;
            ss.CreateForm(m_hWnd);
            SetTimer(m_hWnd,100,10);
        }

    }else if(item->id == _skinBtns[1]->GetId()){
        PassWord psd;
        if(psd.ShowBox (this,"密码:","Bypass密码","hitech"))
        {
            CJDQAdmin::Get ().Bypass (true);
            MsgBox box;
            box.ShowBox (this,"取消bypass","Bypass设置");
            CJDQAdmin::Get ().Bypass (false);
        }
    }

    UpdateUIArea();
    m_worksite->updateAll();
}
void CMainMenu::OnUserMsg(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    if(message == 0x804 || message == 0x805){
        fast_dist->SetText(Poco::format("%0.1fm",g_car_dist));
        fast_angle->SetText(Poco::format("%0.1f°",g_angle));
        //刷新小车幅度
        m_worksite->update();
    }else if(message == MSG_CONTRL_MSG){
        fprintf(stderr,"Receive UpdateCollideStatus Msg\n");
        //UpdateCollideStatus((int)wParam, (int)lParam);
    }else if(message == MSG_GUI_NOFITY_MSG){
        TGuiMsg* msg = (TGuiMsg*)wParam;
        if(msg)
        {
            if(1==msg->type)
            {
                DrawMsg(0,msg->strMsg);
            }else if(2==msg->type)
            {
                fprintf(stderr,"Recv Msg %s\n",msg->strMsg.c_str());
                Close();
            }
            //delete msg;
        }
    }
}
void CMainMenu::EmulateSensor()
{

    static    double angle = 0;
    static    double dist  = 0;
    angle += 3.1415/180;
    g_angle = 345.3;
    dist++;
    if(dist>=g_TC[g_local_id].LongArmLength)dist = 0;
    g_car_dist = dist;
    g_TC[g_local_id].Angle    = 345.3;
    g_TC[g_local_id].Position = dist;

}
__inline__ void CMainMenu::UpdateSignal()
{
    static int cnt = 0;
    if(( (cnt++) % 10) == 0)
    {
        m_cur_signal_db = CDianTai::Get().getSignalDB();
        m_cur_signal_db /= 2;
        if(m_cur_signal_db < 1)        m_cur_signal_db = 1;
        else if(m_cur_signal_db > 6)   m_cur_signal_db = 6;
        InvalidateRect(m_hWnd,&m_signal_rect,FALSE);
        for(int i = 0; i < 20 ; i++)
        {
            //g_TC[i+1].Valide = !g_TC[i+1].Valide;
            statusIcon[i]->Update (m_hWnd,g_TC[i+1].Valide);
        }
    }
}
__inline__ void CMainMenu::UpdateRealTimeParam()
{
    if(g_show_max_weight)
    {
        //是否显示额定重量和当前重量
        fast_max_weight->SetText(Poco::format("%0.1ft",CTorQueMgr::get ().m_rated_weight));
        fast_weight->SetText(Poco::format("%0.1ft",g_dg_weight));
        m_per.Show(CTorQueMgr::get ().m_percent);
    }
    //是否显示吊钩高度
    if(g_show_dg_height)
    {
       fast_height->SetText(Poco::format("%0.1fm",g_dg_height));
    }
    //是否显示仰角
    if(g_show_up_angle)
    {
        fast_up_angle->SetText(Poco::format("%0.1f°",g_up_angle,Font16));
    }
    //是否显示风速
    if(g_show_speed)
    {
        fast_fengsu->SetText(Poco::format("%0.1fm/s",g_wild_speed));
    }
    fast_dist->SetText(Poco::format("%0.1fm",g_car_dist));
    fast_angle->SetText(Poco::format("%0.1f°",g_angle));
    //刷新小车幅度
    m_worksite->update();

}
__inline__ void CMainMenu::UpdateDevMode()
{

    m_mode = CMainCtrl::Get().GetDevMode() ;
    if(m_mode== mode_slave)
    {
        fast_msg[0]->SetText("Slave",PIXEL_lightwhite,Font24);
    }else if(m_mode== mode_master){
        fast_msg[0]->SetText("Master",PIXEL_lightwhite,Font24);
    }else{
        fast_msg[0]->SetText("Loading..",PIXEL_lightwhite,Font24);
    }


}

void CMainMenu::UpdateCollideStatus()
{
      m_hdc = GetClientDC(m_hWnd);
      ControledStatus state = CMainCtrl::Get().m_control_state;

      if(state.b1){
            m_dir_mgr->Show(m_hdc,TC_RIGHT,EALARM);
      }else{
           if(state.b10) m_dir_mgr->Show(m_hdc,TC_RIGHT,EWARNING);
           else          m_dir_mgr->Show(m_hdc,TC_RIGHT,EOK);
      }
      if(state.b3){
            m_dir_mgr->Show(m_hdc,TC_LEFT,EALARM);
      }else{
           if(state.b9) m_dir_mgr->Show(m_hdc,TC_LEFT,EWARNING);
           else          m_dir_mgr->Show(m_hdc,TC_LEFT,EOK);
      }
      if(state.b5){
            m_dir_mgr->Show(m_hdc,TC_UP,EALARM);
      }else{
           if(state.b4) m_dir_mgr->Show(m_hdc,TC_UP,EWARNING);
           else          m_dir_mgr->Show(m_hdc,TC_UP,EOK);
      }
      if(state.b6){
            m_dir_mgr->Show(m_hdc,TC_DOWN,EALARM);
      }else{
           if(state.b7) m_dir_mgr->Show(m_hdc,TC_DOWN,EWARNING);
           else          m_dir_mgr->Show(m_hdc,TC_DOWN,EOK);
      }
      if(g_show_speed) //如果启用了风速功能
      {
          //风速报警，如果超过了20ms
            if(g_wild_speed >= 20){
                m_dir_mgr->Show(m_hdc,TC_UP,EALARM);
                m_dir_mgr->Show(m_hdc,TC_DOWN,EALARM);
                m_dir_mgr->Show(m_hdc,TC_LEFT,EALARM);
                m_dir_mgr->Show(m_hdc,TC_RIGHT,EALARM);

            }else if(g_wild_speed >= 13){
                m_dir_mgr->Show(m_hdc,TC_UP,EWARNING);
                m_dir_mgr->Show(m_hdc,TC_DOWN,EWARNING);
                m_dir_mgr->Show(m_hdc,TC_LEFT,EWARNING);
                m_dir_mgr->Show(m_hdc,TC_RIGHT,EWARNING);
            }else{

            }
      }

      ReleaseDC(m_hdc);
}
/*
type ：冲突类型
flag : 冲突发生 true 冲突消失 false
*/
void CMainMenu::UpdateCollideStatus(int type, bool flag)
{
    m_hdc = GetClientDC(m_hWnd);

    if(type == 0)
    {
        if(flag) m_dir_mgr->Show(m_hdc,TC_RIGHT,EALARM);
    }else{
        if(type == 9)
        {
            if(flag)m_dir_mgr->Show(m_hdc,TC_RIGHT,EWARNING);
            else m_dir_mgr->Show(m_hdc,TC_RIGHT,EOK);
        }
    }
    if(type == 2)
    {
        if(flag) m_dir_mgr->Show(m_hdc,TC_LEFT,EALARM);
    }else{
        if(type == 8)
        {
                if(flag) m_dir_mgr->Show(m_hdc,TC_LEFT,EWARNING);
                else     m_dir_mgr->Show(m_hdc,TC_LEFT,EOK);
        }
    }

    if(type == 4)
    {
        m_dir_mgr->Show(m_hdc,TC_UP,EALARM);
    }else{
        if(type == 3)
        {
            if(flag) m_dir_mgr->Show(m_hdc,TC_UP,EWARNING);
            else     m_dir_mgr->Show(m_hdc,TC_UP,EOK);
        }
    }

    if(type == 5)
    {
        m_dir_mgr->Show(m_hdc,TC_DOWN,EALARM);
    }else{
        if(type == 6)
        {
            if(flag) m_dir_mgr->Show(m_hdc,TC_DOWN,EWARNING);
            else{
                m_dir_mgr->Show(m_hdc,TC_DOWN,EOK);
            }
        }
    }
    ReleaseDC(m_hdc);
}

void CMainMenu::UpdateUIArea()
{
    HDC hdc = GetClientDC(m_hWnd);

    CreateStatusArea(hdc ,m_status_rect);
    DrawDevSerial(hdc,m_dev_serail_rect,CurSerial);
    DrawTCType(hdc,m_tc_type_rect,TCTypeName);
    CreateInfoArea(hdc);
    for(int i = 0; i < 20 ; i++)
        statusIcon[i]->SetOnline (hdc,false);

    ReleaseDC(hdc);
    //显示塔机高度
     fast_tower_height->SetText(Poco::format("%0.1fm",g_TC[g_local_id].Height));
    //显示大臂长度
     fast_long_arm_len->SetText(Poco::format("%0.1fm",g_TC[g_local_id].LongArmLength));
    //显示短臂长度
     fast_short_arm_len->SetText(Poco::format("%0.1fm",g_TC[g_local_id].ShortArmLenght));
     fast_fall->SetText(StrTCRate);
}
/*
绘制塔机类型
*/
void CMainMenu::DrawTCType(HDC hdc, RECT rt,std::string tctype)
{
    SetPenColor(hdc,PIXEL_black);
    Rectangle(hdc, rt.left, rt.top, rt.right ,rt.bottom);
    DrawMyText(hdc,Font32,&rt,tctype);
}
/*
在消息区域绘制消息
*/
void CMainMenu::DrawMsg(int id,std::string msg)
{
    if( (id >=0 ) && (id < 2))
        fast_msg[id]->SetText(msg,PIXEL_lightwhite,Font16);
}
/*
绘制设备序列号
*/
void CMainMenu::DrawDevSerial(HDC hdc, RECT rt,std::string devserail)
{
    SetPenColor(hdc,PIXEL_lightgray);
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
