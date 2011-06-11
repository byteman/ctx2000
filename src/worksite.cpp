#include "worksite.h"
#include "worksiteitem.h"
#include "BmpRes.h"
#include "tajidbmgr.h"
#include <iniFile.h>
#include <Poco/Format.h>
#include "comdata.h"
#include <yatengine.h>
#include "mainctrl.h"
#include "MsgBox.h"
using namespace TelEngine;
extern TTjRecord g_TC[21];
static COMM_CTRL_DESC lablectrls[] = {
    {50,  50, 100,30,"Left-Down:"},
    {50,  80, 30,30,"x1:"},
    {160, 80, 30,30,"y1:"},

    {50,  120, 100,30,"Right-Down:"},
    {50,  150, 30, 30,"x2:"},
    {160, 150, 30, 30,"y2:"},

    {50,  190, 100,30,"Right-Up:"},
    {50,  220, 30,30,"x3:"},
    {160, 220, 30,30,"y3:"},

    {50,  260, 100,30,"Left-Up:"},
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
    {550,  330, 80,30,"FD time(s):"},
    {650,  330, 80,30,"Brake time(s):"},
};
static COMM_CTRL_DESC editctrls[] = {
    {105,  78,  80,30,"0"},
    {105, 109,  80,30,"0"},

    {105, 168, 80,30,"0"},
    {105, 199, 80,30,"0"},

    {105, 259, 80,30,"0"},
    {105, 287, 80,30,"0"},

    {105, 348, 80,30,"0"},
    {105, 379, 80,30,"0"},
//===============
    {318, 78,  100,30,"0"},

    {318, 168,  80,30,"1"},
    {478, 168,  80,30,"2"},

    {318, 199,  80,30,"3"},
    {478, 199,  80,30,"4"},

    {318, 259,  80,30,"5"},
    {318, 348,  80,30,"6"},

    {608,  78,  100,30, "0"},
    {608,  168, 100,30,"1"},
    {608,  259, 100,30,"2"},
    //add safe angle
    {478,  259, 100,30,"3"},
    //FD
    {478,  348, 80,30,"10"},
    //brake
    {608,  348, 80,30,"10"},
};
static const char* mmenu_bmps[] = {
        PWSBACKGROUND,
        PCOMM_MDY_BTN,
        PCOMM_SAVE_BTN,
        PCOMM_CLOSE_BTN,
        PCOMM_RET_BTN
};
static SKIN_BUTTON_DESC skinctrls[] = {
        {1,BUTTON_DEFAULE_X,BUTTON_DEFAULE_Y},
        {2,BUTTON_SAVE_X,BUTTON_SAVE_Y},
        {3,BUTTON_EXIT_X,BUTTON_EXIT_Y},
        {4,BUTTON_RETURN_XX,BUTTON_SAVE_Y}
};
static const char* icon_path[] = {

};
#include "SoftKeyboard.h"
extern SoftKeyboard* skt;
void  CWorkSite::OnCommCtrlNotify(HWND hwnd, int id, int nc)
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
CWorkSite::CWorkSite()
{
    if(!LoadRes(&mmenu_bmps[0],ARRAY_SIZE(mmenu_bmps,char*)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
    /*
    for(int i = 0 ;i < (sizeof(lablectrls)/sizeof(COMM_CTRL_DESC));i++)
    {
        lables[i] = new CStatic(&lablectrls[i],this);
    }
    */
    for(int i = 0 ;i < (sizeof(editctrls)/sizeof(COMM_CTRL_DESC)) ;i++)
    {
        edits[i] = new CEdit(&editctrls[i],this);
    }
    btn_mdy     = new CSkinButton(&skinctrls[0],this);
    btn_save    = new CSkinButton(&skinctrls[1],this);
    btn_exit    = new CSkinButton(&skinctrls[2],this);;
    btn_ret     = new CSkinButton(&skinctrls[3],this);;
    InitSkinHeader("WET_param");
}
CWorkSite::~CWorkSite()
{	

}
void    CWorkSite::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);
    EndPaint(hWnd,hdc);
}
void    CWorkSite::OnShow()
{

    for(int i = 0; i <4;i++)
    {
        edits[i*2+0]->SetText(Poco::format("%0.2f",  wksp[i][0]));
        edits[i*2+1]->SetText(Poco::format("%0.2f",  wksp[i][1]));
    }
    edits[8]->SetText(Poco::format("%0.2f",YNAngle));
    edits[9]->SetText(Poco::format("%0.2f",czwzb[0][0]));
    edits[10]->SetText(Poco::format("%0.2f",czwzb[0][1]));
    edits[11]->SetText(Poco::format("%0.2f",czwzb[1][0]));
    edits[12]->SetText(Poco::format("%0.2f",czwzb[1][1]));

    edits[13]->SetText(Poco::format("%0.2f",VStopDis));
    edits[14]->SetText(Poco::format("%0.2f",VWarnDis));

    edits[15]->SetText(Poco::format("%0.2f",BrakeDis));
    edits[16]->SetText(Poco::format("%0.2f",DangerDis));
    edits[17]->SetText(Poco::format("%0.2f",WarnDis));
    edits[18]->SetText(Poco::format("%0.2f",AddAngle));
    edits[19]->SetText(Poco::format("%d",FDTime));
    edits[20]->SetText(Poco::format("%d",BrakeTime));
}
void    CWorkSite::OnButtonClick(skin_item_t* item)
{

    if(item->id == btn_mdy->GetId())
    {

    }
    else if(item->id == btn_save->GetId())
    {
        bool ok;
        for(int i = 0 ;i < 4;i++)
        {
            wksp[i][0] = edits[i*2+0]->GetFloatValue(ok);
            wksp[i][1] = edits[i*2+1]->GetFloatValue(ok);
        }
        YNAngle = edits[8]->GetFloatValue(ok);
        for(int i = 0 ;i < 2;i++)
        {
            czwzb[i][0] =  edits[i*2+9]->GetFloatValue(ok);
            czwzb[i][1] =  edits[i*2+10]->GetFloatValue(ok);
        }

        VStopDis    = edits[13]->GetFloatValue(ok);
        VWarnDis    = edits[14]->GetFloatValue(ok);
        BrakeDis    = edits[15]->GetFloatValue(ok);
        DangerDis   = edits[16]->GetFloatValue(ok);
        WarnDis     = edits[17]->GetFloatValue(ok);
        AddAngle    = edits[18]->GetFloatValue(ok);

        FDTime      = edits[19]->GetIntValue(ok);
        BrakeTime   = edits[20]->GetIntValue(ok);

        edits[13]->SetText(Poco::format("%0.2f",VStopDis));
        edits[14]->SetText(Poco::format("%0.2f",VWarnDis));
        edits[15]->SetText(Poco::format("%0.2f",BrakeDis));
        edits[16]->SetText(Poco::format("%0.2f",DangerDis));
        edits[17]->SetText(Poco::format("%0.2f",WarnDis));
        edits[18]->SetText(Poco::format("%0.2f",AddAngle));

        edits[19]->SetText(Poco::format("%d",FDTime));
        edits[20]->SetText(Poco::format("%d",BrakeTime));

        CMainCtrl::Get().SaveSiteInfo();
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
    }

}
