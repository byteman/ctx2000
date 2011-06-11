#include "divparam.h"
#include "divparamitem.h"
#include "BmpRes.h"
#include "tajidbmgr.h"
#include <iniFile.h>
#include <Poco/Format.h>
#include "comdata.h"
#include <yatengine.h>
#include <Poco/String.h>
#include "MsgBox.h"
using namespace TelEngine;
extern TTjRecord g_TC[21];
static COMM_CTRL_DESC lablectrls[] = {
    {50,  50, 100,30,"编号:"},
    {50,  50, 100,30,"高度:"},
    {50,  80, 30,30,"x1:"},
    {160, 80, 30,30,"y1:"},
    {50,  120, 100,30,"x2:"},
    {50,  150, 30, 30,"y2:"},

    {50,  150, 30, 30,"x3:"},
    {160, 150, 30, 30,"y3:"},

    {50,  190, 100,30,"x4:"},
    {50,  220, 30,30,"y4:"},
    {50,  220, 30,30,"x5:"},
    {160, 220, 30,30,"y5:"},
    {50,  260, 100,30,"x6:"},
    {50,  260, 100,30,"y6:"},
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
    {230,  60,  80,30,"0"},
    {230,  110,  80,30,"0"},
    {230,  160,  80,30,"0"},

    {230,  210, 80,30,"0"},
    {230,  260, 80,30,"0"},

    {230,  310, 80,30,"0"},
    {230,  360, 80,30,"0"},
    {520, 60, 80,30,"0"},

    {520,  110, 80,30,"0"},
    {520, 160, 80,30,"0"},
//===============
    {520, 210,  100,30,"0"},
    {520, 260,  100,30,"0"},
    {520, 310,  100,30,"0"},
    {520, 360,  100,30,"0"},

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
        PDIVBACKGROUND,
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
        {5,710,80},
        {6,710,280},
        {7,355,58+8*29}
};
static const char *icon_path[] =
{

};

static POS IconsPos[] =
{

};
#include "SoftKeyboard.h"
extern SoftKeyboard* skt;
void  CDivParam::OnCommCtrlNotify(HWND hwnd, int id, int nc)
{
    if(nc==EN_SETFOCUS)
    {
        fprintf(stderr,"setfocus\n");
        if(skt)
        {
            //fprintf(stderr,"t9show\n");
            skt->T9_Show(true);
        }
    }
}
CDivParam::CDivParam()
{
    if(!LoadRes(&mmenu_bmps[0],ARRAY_SIZE(mmenu_bmps,char*)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
#define V_SPAN 29
#define H      24
#define Y      100
    /*
    for(int i = 0 ;i < (sizeof(lablectrls)/sizeof(COMM_CTRL_DESC));i++)
    {
        if(i >= 7)
        {
            lablectrls[i].x = 350;
            lablectrls[i].y = Y+(i-7)*V_SPAN;
        }else{
            lablectrls[i].x = 100;
            lablectrls[i].y = Y+i*V_SPAN;
        }

        lablectrls[i].w = 100;
        lablectrls[i].h = H;
        lables[i] = new CStatic(&lablectrls[i],this);
    }
    */
    for(int i = 0 ;i < (sizeof(editctrls)/sizeof(COMM_CTRL_DESC)) ;i++)
    {
        /*
        if(i >= 7)
        {
            editctrls[i].x = 450;
            editctrls[i].y = Y+(i-7)*V_SPAN;
        }else{
            editctrls[i].x = 200;
            editctrls[i].y = Y+i*V_SPAN;
        }
        */
        editctrls[i].h = H;
        editctrls[i].w = 150;
        edits[i] = new CEdit(&editctrls[i],this);
    }
    btn_mdy     = new CSkinButton(&skinctrls[0],this);
    btn_save    = new CSkinButton(&skinctrls[1],this);
    btn_exit    = new CSkinButton(&skinctrls[2],this);
    btn_ret     = new CSkinButton(&skinctrls[3],this);
    btn_next    = new CSkinButton(&skinctrls[4],this);
    btn_prev    = new CSkinButton(&skinctrls[5],this);

    InitSkinHeader("WET_param");
    m_tc_id = 1;
}
CDivParam::~CDivParam()
{	

}
void    CDivParam::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);
    EndPaint(hWnd,hdc);
}
void    CDivParam::OnShow()
{
    Update(1);
}
#include <Poco/Format.h>
void    CDivParam::Update(int id)
{

    fprintf(stderr,"update=%d\n",id);
    edits[0]->SetIntText(id);
    id--;
    edits[1]->SetFloatText(wba[id].h);
    for(int i = 0; i < 6; i++)
    {
        //3 point
        if(i < wba[id].VertexNum)
        {
            fprintf(stderr,"i=%d\n",i);
            edits[2+i*2]->SetText(Poco::format("%0.2f",wba[id].Pointxy[i][0]),false);
            edits[3+i*2]->SetText(Poco::format("%0.2f",wba[id].Pointxy[i][1]),false);
            //edits[2+i*2]->SetFloatText();
            //edits[3+i*2]->SetFloatText(wba[id].Pointxy[i][1]);
        }
        else
        {
            edits[2+i*2]->SetText("");
            edits[3+i*2]->SetText("");
        }
    }/*
    edits[2]->SetFloatText(wba[id].);
    edits[3]->SetFloatText(g_TC[id].y);
    edits[4]->SetFloatText(g_TC[id].Height);
    edits[5]->SetFloatText(g_TC[id].LongArmLength);
    edits[6]->SetFloatText(g_TC[id].ShortArmLenght);
    edits[7]->SetFloatText(g_TC[id].Rs);
    edits[8]->SetFloatText(g_TC[id].L1);
    edits[9]->SetFloatText(g_TC[id].L2);
    edits[10]->SetFloatText(g_TC[id].a0);
    */
}
#include "mainctrl.h"
#include <Poco/NumberParser.h>
void    CDivParam::SaveTC(int id)
{
    bool ok1 =false,ok2=false;

    id--;
    wba[id].h =  edits[1]->GetFloatValue(ok1);
    int num = 0;
    for(int i = 0; i < 6; i++)
    {
        double x;
        if(Poco::NumberParser::tryParseFloat(edits[2*i+2]->GetText(),x))
        {
            wba[id].Pointxy[i][0] = x;
            if(Poco::NumberParser::tryParseFloat(edits[2*i+3]->GetText(),x))
            {
                wba[id].Pointxy[i][1] = x;
                num++;
            }
        }
        //wba[id].Pointxy[i][0] =  edits[2*i+2]->GetFloatValue(ok1);
        //wba[id].Pointxy[i][1] =  edits[2*i+3]->GetFloatValue(ok2);
        //if(ok1&&ok2)num++;
    }
    wba[id].VertexNum=num;
    fprintf(stderr,"savebuild id =%d,x=%0.2f,y=%0.2f num=%d\n",id,wba[id].Pointxy[0][0],wba[id].Pointxy[0][1],num);
    CMainCtrl::Get().SaveBuildingInfo();

}
void    CDivParam::OnButtonClick(skin_item_t* item)
{

    if(item->id == btn_mdy->GetId())
    {

    }
    else if(item->id == btn_save->GetId())
    {

        SaveTC(m_tc_id);
        MsgBox box;
        box.ShowBox(this,"参数保存成功","信息提示");

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
        if(m_tc_id > wbNum)
            m_tc_id = 1;
        Update(m_tc_id);

    } else if(item->id == btn_next->GetId())
    {
        m_tc_id--;
        if(m_tc_id < 1)
            m_tc_id = (wbNum);
        Update(m_tc_id);
    }

}
