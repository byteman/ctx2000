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
#define EDIT_ANGLE   {20,285,160,30,"20.8"}
#define EDIT_DIST    {20,335,160,30,"12.4"}
#define EDIT_UP_ANGL {100,370,80,30,"12.4"}
//#define EDIT_WEIGHT  {20,350,160,30,"12.4"}
#define EDIT_SPEED   {710,350,80,30,"12.4"}
#define EDIT_HEIGHT  {710,410,80,30,"12.4"}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static const char* mmenu_bmps[] = {
        "comm/ctxbk.png",
        PSCALE_BTN,
        PQUERY_BTN,
        PPRESET_BTN,
        PSYSCFG_BTN,
        PWETCFG_BTN,
        PHELP_BTN
};
static SKIN_BUTTON_DESC SkinBtns[] = {
    //SKIN_BUTTON_SCALE,
    //SKIN_BUTTON_QUERY ,
    //SKIN_BUTTON_PRESET,
    SKIN_BUTTON_WETCFG,
    SKIN_BUTTON_SYSCFG,
    //SKIN_BUTTON_HELP
};
#define Client_Height 72
static COMM_CTRL_DESC areactrls[] = {
    {200,  Client_Height+5 , 500,400,""},
};
static COMM_CTRL_DESC commctrls[] = {
    EDIT_ANGLE,
    EDIT_DIST,
    {710,150,80,30,"0"},{710,210,80,30,"0"},{710,270,80,30,"0"},
    {710,120,80,30,"H"},{710,190,80,30,"R"},{710,250,80,30,"R-"},
    //EDIT_WEIGHT,
    EDIT_UP_ANGL,
    EDIT_SPEED,
    EDIT_HEIGHT,

    {710,330,80,30,"风速:"},{710,390,80,30,"吊钩高度:"},{10,375,80,30,"动臂仰角:"}
};
static EDevStatus g_status[20];


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



    for (int i = 0; i < 2; i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtns[i],this);
    }
    //edt_angle = new CEdit(&commctrls[0],this);
    //edt_dist  = new CEdit(&commctrls[1],this);
    lbl_angle = new CStatic(&commctrls[0],this);
    lbl_dist  = new CStatic(&commctrls[1],this);


    edt_height = new CEdit(&commctrls[2],this);
    edt_long_arm_len= new CEdit(&commctrls[3],this);
    edt_short_arm_len= new CEdit(&commctrls[4],this);




    if(m_show_up_angle)
    {
        new CStatic(&commctrls[13],this);
        edt_up_angle= new CEdit(&commctrls[8],this);
    }
    if(m_show_speed)
    {
        new CStatic(&commctrls[11],this);
        edt_fengsu = new CEdit(&commctrls[9],this);

    }
    if(m_show_dg_height)
    {
        new CStatic(&commctrls[12],this);
        edt_dg_height = new CEdit(&commctrls[10],this);
    }
    lbl_rights[0] = new CStatic(&commctrls[5],this);
    lbl_rights[1] = new CStatic(&commctrls[6],this);
    lbl_rights[2] = new CStatic(&commctrls[7],this);

    Font24 = CFontMgr::Get().GetFont("stxinwei",24);
    assert(Font24 != NULL);
    Font24->setFontColor(RGB2Pixel(HDC_SCREEN,255,0,0));

    fprintf(stderr,"sbc=%s,mbc=%s\n",Font24->pfont->sbc_devfont->name,Font24->pfont->mbc_devfont->name);


    SetRect(&m_status_rect,200,0,800,Client_Height);
    SetRect(&m_dev_serail_rect,0,0,200,Client_Height);


    for(int i = 0; i < 1; i++)
    {
        m_areas[i] = new CStatic(&areactrls[i],this);
    }

    m_worksite = new CFormWorksite(m_areas[0],areactrls[0].w,areactrls[0].h);

    m_dir_mgr = new CDirStatusMgr(30,Client_Height+25);
    m_angle.LoadFile("ctx2000/angle.jpg");
    m_dist.LoadFile("ctx2000/dist.jpg");
    InitSkinHeader("MainMenu");



}

CMainMenu::~CMainMenu()
{

}
void CMainMenu::DrawDevSerial(HDC hdc, RECT rt,std::string devserail)
{
    SetPenColor(hdc,PIXEL_lightgray);
    Rectangle(hdc, rt.left, rt.top, rt.right ,rt.bottom);

    DrawMyText(hdc,Font24,&rt,devserail);

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
#include <math.h>
int paintcircle(int x,int r)
{
    int y = (int)sqrt(r*r -x*x);
    fprintf(stderr,"x=%d y=%d r=%d\n",x,y,r);
    return y;

}

#include <Poco/Format.h>

void CMainMenu::OnShow()
{
    m_worksite->init(m_hWnd);
}
void CMainMenu::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);
    CreateStatusArea(hdc ,m_status_rect);
    DrawDevSerial(hdc,m_dev_serail_rect,"No.C8001");

    for(int i = 0; i < 2 ;i++)
        statusIcon[i]->SetStatus(hdc,g_status[i]);
    for(int i = 2; i < 6 ;i++)
        statusIcon[i]->SetStatus(hdc,g_status[i]);
    for(int i = 6; i < 10 ;i++)
        statusIcon[i]->SetStatus(hdc,g_status[i]);

    m_dir_mgr->Show(hdc,EOK,EOK,EWARNING,EALARM);
    m_dir_mgr->Show(hdc,EOK,EOK,EWARNING,EALARM);

    m_angle.Show(hdc,80,245,1);
    m_dist.Show(hdc, 58,310,1);
    EndPaint(hWnd, hdc);
}
void CMainMenu::OnTimer(int ID)
{
    if (m_msg_delay > 0)
    {
        if (--m_msg_delay <= 0)
        {
            UpdateWindow(m_hWnd, true);
            m_msg_delay = 0;
        }
    }    
    static int cnt =0;
    static double angle = 0;
    static int dist =0;
    for(int i = 0; i < 20 ;i++)
    {
        cnt++;
        g_status[i] = EDevStatus(cnt%3);
    }

    int localid = CTajiDbMgr::Get().GetLocalIndex();
    lbl_dist->SetText(Poco::format("%0.2f",g_qtzs[localid].carrier_pos));
    lbl_angle->SetText(Poco::format("%0.2f",g_qtzs[localid].long_arm_angle));

    if(m_show_up_angle)
    {
        edt_up_angle->SetFloatText(g_up_angle,3);
    }
    if(m_show_speed)
    {
        edt_fengsu->SetFloatText(g_speed,2);
    }
    if(m_show_dg_height)
    {
        edt_dg_height->SetFloatText(434.2,2);
    }
    m_worksite->update();
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

    }else if(item->id == _skinBtns[3]->GetId()){

    }else if(item->id == _skinBtns[4]->GetId()){

    }else if(item->id == _skinBtns[5]->GetId()){

    }

}
