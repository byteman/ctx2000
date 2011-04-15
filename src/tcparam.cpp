#include "tcparam.h"
#include "tcparamitem.h"
#include "BmpRes.h"
#include "tajidbmgr.h"
#include <iniFile.h>
#include <Poco/Format.h>
#include "comdata.h"
#include <yatengine.h>
#include <Poco/String.h>
#include "mainctrl.h"
using namespace TelEngine;
extern TTjRecord g_TC[21];
static COMM_CTRL_DESC lablectrls[] = {
    {50,  50, 100,30,"塔机编号:"},
    {50,  80, 30,30,"设备名:"},
    {160, 80, 30,30,"x坐标:"},
    {50,  120, 100,30,"y坐标:"},

    {50,  150, 30, 30,"塔机高度:"},
    {50,  150, 30, 30,"大臂长度:"},
    {160, 150, 30, 30,"短臂长度:"},

    {50,  190, 100,30,"最小距离:"},
    {50,  220, 30,30,"动臂式:"},
    {50,  220, 30,30,"最大仰角:"},
    {160, 220, 30,30,"最小仰角:"},
    {50,  260, 100,30,"结构参数:"},
    /*
    {50,  290, 30,30,"x4:"},
    {160, 290, 30,30,"y4:"},
//====================
    {290,  50,  100,30,"Y-->N Angle:"},

    {290,  120, 100,30,"Anchor Point:"},

    {290,  150, 30,30,"x1:"},
    {400,  150, 30,30,"y1:"},

    {290,  210, 30,30,"x2:"},
    {400,  210, 30,30,"y2:"},

    {290,  260, 200,30,"Vehicle Stop Distance:"},
    {290,  330, 200,30,"Vehicle Lowspeed Distance:"},

    {550,  50,  200,30, "Brake Distance:"},
    {550,  120, 200,30,"Danger Distance:"},
    {550,  190, 200,30,"Warn Distance:"},
    {550,  260, 200,30,"Add Safe Angle:"},
        */
};
static COMM_CTRL_DESC editctrls[] = {
    {80,  80,  80,30,"0"},
    {190, 80,  80,30,"0"},

    {80,  150, 80,30,"0"},
    {190, 150, 80,30,"0"},

    {80,  220, 80,30,"0"},
    {80,  220, 80,30,"0"},
    {190, 220, 80,30,"0"},

    {80,  290, 80,30,"0"},
    {190, 290, 80,30,"0"},
//===============
    {320, 80,  100,30,"0"},
    {320, 80,  100,30,"0"},

   // {320, 150,  80,30,"1"},
    /*
    {430, 150,  80,30,"2"},

    {320, 220,  80,30,"3"},
    {430, 220,  80,30,"4"},

    {320, 290,  80,30,"5"},
    {320, 360,  80,30,"6"},

    {550,  80,  100,30, "0"},
    {550,  150, 100,30,"1"},
    {550,  220, 100,30,"2"},
    {550,  290, 100,30,"3"},
        */
};
static const char* mmenu_bmps[] = {
        PCOMM_BACKGROUND,
        PCOMM_MDY_BTN,
        PCOMM_SAVE_BTN,
        PCOMM_CLOSE_BTN,
        PCOMM_RET_BTN,
        PUP_BTN,
        PDOWN_BTN,
        PCOMM_CHECK_BTN
};
static SKIN_BUTTON_DESC skinctrls[] = {
        {1,BUTTON_DEFAULE_X,BUTTON_DEFAULE_Y},
        {2,BUTTON_SAVE_X,BUTTON_SAVE_Y},
        {3,BUTTON_EXIT_X,BUTTON_EXIT_Y},
        {4,BUTTON_RETURN_XX,BUTTON_SAVE_Y},
        {5,550,100},
        {6,550,300},
        {7,355,58+8*29}
};
static const char* icon_path[] = {

};

CTCParam::CTCParam()
{
    if(!LoadRes(&mmenu_bmps[0],ARRAY_SIZE(mmenu_bmps,char*)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
#define V_SPAN 29
#define H      24
    for(int i = 0 ;i < (sizeof(lablectrls)/sizeof(COMM_CTRL_DESC));i++)
    {
        lablectrls[i].x = 250;
        lablectrls[i].y = 58+i*V_SPAN;
        lablectrls[i].w = 100;
        lablectrls[i].h = H;
        lables[i] = new CStatic(&lablectrls[i],this);
    }
    for(int i = 0 ;i < (sizeof(editctrls)/sizeof(COMM_CTRL_DESC)) ;i++)
    {
        editctrls[i].x = 250+105;
        editctrls[i].y = 58+i*V_SPAN;
        editctrls[i].h = H;
        editctrls[i].w = 150;
        if(i >= 8)
        {
            editctrls[i].y += V_SPAN;
        }
        edits[i] = new CEdit(&editctrls[i],this);
    }
    btn_mdy     = new CSkinButton(&skinctrls[0],this);
    btn_save    = new CSkinButton(&skinctrls[1],this);
    btn_exit    = new CSkinButton(&skinctrls[2],this);
    btn_ret     = new CSkinButton(&skinctrls[3],this);
    btn_next    = new CSkinButton(&skinctrls[4],this);
    btn_prev    = new CSkinButton(&skinctrls[5],this);
    chk_dynamic = new CSkinChkButton(&skinctrls[6],this);
    InitSkinHeader("WET_param");
    m_tc_id = 1;
}
CTCParam::~CTCParam()
{	

}
void    CTCParam::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);
    EndPaint(hWnd,hdc);
}
void    CTCParam::OnShow()
{
    Update(1);
}
void    CTCParam::Update(int id)
{
    edits[0]->SetIntText(id);
    std::string serial = g_TC[id].Serial;
    edits[1]->SetText(Poco::trim(serial));
    edits[2]->SetFloatText(g_TC[id].x);
    edits[3]->SetFloatText(g_TC[id].y);
    edits[4]->SetFloatText(g_TC[id].Height);
    edits[5]->SetFloatText(g_TC[id].LongArmLength);
    edits[6]->SetFloatText(g_TC[id].ShortArmLenght);

    edits[7]->SetFloatText(g_TC[id].Rs);
    edits[8]->SetFloatText(g_TC[id].L1);
    edits[9]->SetFloatText(g_TC[id].L2);
    edits[10]->SetFloatText(g_TC[id].a0);
    if(g_TC[id].Dyna)
        chk_dynamic->SetCheckStatus(true);
    else
        chk_dynamic->SetCheckStatus(false);

}
void    CTCParam::SaveTC(int id)
{
    bool ok =false;

    memset(g_TC[id].Serial,0,20);
    edits[1]->GetText().copy(g_TC[id].Serial,20);
    g_TC[id].x =  edits[2]->GetFloatValue(ok);
    g_TC[id].y =  edits[3]->GetFloatValue(ok);
    g_TC[id].Height =  edits[4]->GetFloatValue(ok);
    g_TC[id].LongArmLength =  edits[5]->GetFloatValue(ok);
    g_TC[id].ShortArmLenght =  edits[6]->GetFloatValue(ok);
    g_TC[id].Rs =  edits[7]->GetFloatValue(ok);

    g_TC[id].L1 =  edits[8]->GetFloatValue(ok);
    g_TC[id].L2 =  edits[9]->GetFloatValue(ok);
    g_TC[id].a0 =  edits[10]->GetFloatValue(ok);
    if(chk_dynamic->GetCheckStatus())
        g_TC[id].Dyna = 1;
    else
        g_TC[id].Dyna = 0;


}
void    CTCParam::OnButtonClick(skin_item_t* item)
{

    if(item->id == btn_mdy->GetId())
    {

    }
    else if(item->id == btn_save->GetId())
    {

        SaveTC(m_tc_id);
        CMainCtrl::Get().SaveTowerCraneInfo();
    }
    else if(item->id == btn_exit->GetId())
    {
        Close();
    }
    else if(item->id == btn_ret->GetId())
    {
        Close();
    } else if(item->id == btn_prev->GetId())
    {
        m_tc_id++;
        if(m_tc_id > 20)
            m_tc_id = 1;
        Update(m_tc_id);

    } else if(item->id == btn_next->GetId())
    {
        m_tc_id--;
        if(m_tc_id < 1)
            m_tc_id = 20;
        Update(m_tc_id);
    }

}
