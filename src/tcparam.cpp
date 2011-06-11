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
#include "MsgBox.h"
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
    //===========taji one==========
    {150,  56,  80,30,"0"},
    {150,  91,  80,30,"0"},

    {150,  120, 80,30,"0"},
    {150,  150, 80,30,"0"},

    {150,  180, 80,30,"0"},
    {150,  210, 80,30,"0"},

    {150,  240, 80,30,"0"},

    {150,  270, 80,30,"0"},
    {150,  330, 80,30,"0"},

    {150,  360,  100,30,"0"},
    {150,  390,  100,30,"0"},
    //===========taji two============

    {350,  56,  80,30,"0"},
    {350,  94,  80,30,"0"},

    {350,  120, 80,30,"0"},
    {350,  150, 80,30,"0"},

    {350,  180, 80,30,"0"},
    {350,  210, 80,30,"0"},

    {350,  240, 80,30,"0"},

    {350,  270, 80,30,"0"},
    {350,  330, 80,30,"0"},

    {350,  360,  100,30,"0"},
    {350,  390,  100,30,"0"},
    //===========taji three==========
    {570,  56,  80,30,"0"},
    {570,  94,  80,30,"0"},

    {570,  120, 80,30,"0"},
    {570,  150, 80,30,"0"},

    {570,  180, 80,30,"0"},
    {570,  210, 80,30,"0"},

    {570,  240, 80,30,"0"},

    {570,  270, 80,30,"0"},
    {570,  330, 80,30,"0"},

    {570,  360,  100,30,"0"},
    {570,  390,  100,30,"0"},

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
        PTJBACKGROUND,
        PCOMM_MDY_BTN,
        PCOMM_SAVE_BTN,
        PCOMM_CLOSE_BTN,
        PCOMM_RET_BTN,
        PUP_BTN,
        PDOWN_BTN,
        PCOMM_CHECK_BTN
};
static SKIN_BUTTON_DESC skinctrls[] = {
        {1,680,330},
        {2,680,370},
        {3,BUTTON_EXIT_X,BUTTON_EXIT_Y},
        {4,680,410},
        {5,710,80},
        {6,710,240},
        {7,135,305},
        {7,345,305},
        {7,555,305}
};
static const char* icon_path[] = {

};
#include "SoftKeyboard.h"
extern SoftKeyboard* skt;
void  CTCParam::OnCommCtrlNotify(HWND hwnd, int id, int nc)
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
CTCParam::CTCParam()
{
    if(!LoadRes(&mmenu_bmps[0],ARRAY_SIZE(mmenu_bmps,char*)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
#define V_SPAN 29
#define H      24
    /*
    for(int i = 0 ;i < (sizeof(lablectrls)/sizeof(COMM_CTRL_DESC));i++)
    {
        lablectrls[i].x = 250;
        lablectrls[i].y = 58+i*V_SPAN;
        lablectrls[i].w = 100;
        lablectrls[i].h = H;
        lables[i] = new CStatic(&lablectrls[i],this);
    }
    */
    for(int i = 0 ;i < (sizeof(editctrls)/sizeof(COMM_CTRL_DESC)) ;i++)
    {
        editctrls[i].h = H;
        editctrls[i].w = 150;
        edits[i] = new CEdit(&editctrls[i],this);
    }
    btn_mdy      = new CSkinButton(&skinctrls[0],this);
    btn_save     = new CSkinButton(&skinctrls[1],this);
    btn_exit     = new CSkinButton(&skinctrls[2],this);
    btn_ret      = new CSkinButton(&skinctrls[3],this);
    btn_next     = new CSkinButton(&skinctrls[4],this);
    btn_prev     = new CSkinButton(&skinctrls[5],this);
    chk_dynamic1 = new CSkinChkButton(&skinctrls[6],this);
    chk_dynamic2 = new CSkinChkButton(&skinctrls[7],this);
    chk_dynamic3 = new CSkinChkButton(&skinctrls[8],this);
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
    Update(1,1);
    Update(2,2);
    Update(3,3);
}
void    CTCParam::Update(int id,int where)
{
    int pos;
    pos = (where-1)*11;
    edits[0+pos]->SetIntText(id);
    std::string serial = g_TC[id].Serial;
    edits[1+pos]->SetText(Poco::trim(serial));
    edits[2+pos]->SetFloatText(g_TC[id].x);
    edits[3+pos]->SetFloatText(g_TC[id].y);
    edits[4+pos]->SetFloatText(g_TC[id].Height);
    edits[5+pos]->SetFloatText(g_TC[id].LongArmLength);
    edits[6+pos]->SetFloatText(g_TC[id].ShortArmLenght);

    edits[7+pos]->SetFloatText(g_TC[id].Rs);
    edits[8+pos]->SetFloatText(g_TC[id].L1);
    edits[9+pos]->SetFloatText(g_TC[id].L2);
    edits[10+pos]->SetFloatText(g_TC[id].a0);
    switch(where)
    {
        case 1:
        {
          if(g_TC[id].Dyna)
              chk_dynamic1->SetCheckStatus(true);
          else
              chk_dynamic1->SetCheckStatus(false);
          break;
        }
        case 2:
        {
          if(g_TC[id].Dyna)
              chk_dynamic2->SetCheckStatus(true);
          else
              chk_dynamic2->SetCheckStatus(false);
          break;
        }
        case 3:
        {
          if(g_TC[id].Dyna)
              chk_dynamic3->SetCheckStatus(true);
          else
              chk_dynamic3->SetCheckStatus(false);
          break;
        }
        default:
            break;
    }

}
void    CTCParam::SaveTC(int id)
{
    bool ok =false;
    int pos;
    if(id>7) return; //error return
    if(id>0)
    pos = (id-1)*3+1;
    memset(g_TC[pos].Serial,0,20);
    edits[1]->GetText().copy(g_TC[pos].Serial,20);
    g_TC[pos].x =  edits[2]->GetFloatValue(ok);
    g_TC[pos].y =  edits[3]->GetFloatValue(ok);
    g_TC[pos].Height =  edits[4]->GetFloatValue(ok);
    g_TC[pos].LongArmLength  =  edits[5]->GetFloatValue(ok);
    g_TC[pos].ShortArmLenght =  edits[6]->GetFloatValue(ok);
    g_TC[pos].Rs =  edits[7]->GetFloatValue(ok);

    g_TC[pos].L1 =  edits[8]->GetFloatValue(ok);
    g_TC[pos].L2 =  edits[9]->GetFloatValue(ok);
    g_TC[pos].a0 =  edits[10]->GetFloatValue(ok);
    if(chk_dynamic1->GetCheckStatus())
        g_TC[pos].Dyna = 1;
    else
        g_TC[pos].Dyna = 0;
    //========refresh channel 2====
    memset(g_TC[pos+1].Serial,0,20);
    edits[12]->GetText().copy(g_TC[pos+1].Serial,20);
    g_TC[pos+1].x =  edits[13]->GetFloatValue(ok);
    g_TC[pos+1].y =  edits[14]->GetFloatValue(ok);
    g_TC[pos+1].Height =  edits[15]->GetFloatValue(ok);
    g_TC[pos+1].LongArmLength  =  edits[16]->GetFloatValue(ok);
    g_TC[pos+1].ShortArmLenght =  edits[17]->GetFloatValue(ok);
    g_TC[pos+1].Rs =  edits[18]->GetFloatValue(ok);

    g_TC[pos+1].L1 =  edits[19]->GetFloatValue(ok);
    g_TC[pos+1].L2 =  edits[20]->GetFloatValue(ok);
    g_TC[pos+1].a0 =  edits[21]->GetFloatValue(ok);
    if(chk_dynamic2->GetCheckStatus())
        g_TC[pos+1].Dyna = 1;
    else
        g_TC[pos+1].Dyna = 0;
    //========refresh channel 3====
    if((pos+2)>20)
    {
       ;
    }
    else
    {
        memset(g_TC[pos+2].Serial,0,20);
        edits[23]->GetText().copy(g_TC[pos+2].Serial,20);
        g_TC[pos+2].x =  edits[24]->GetFloatValue(ok);
        g_TC[pos+2].y =  edits[25]->GetFloatValue(ok);
        g_TC[pos+2].Height =  edits[26]->GetFloatValue(ok);
        g_TC[pos+2].LongArmLength  =  edits[27]->GetFloatValue(ok);
        g_TC[pos+2].ShortArmLenght =  edits[28]->GetFloatValue(ok);
        g_TC[pos+2].Rs =  edits[29]->GetFloatValue(ok);

        g_TC[pos+2].L1 =  edits[30]->GetFloatValue(ok);
        g_TC[pos+2].L2 =  edits[31]->GetFloatValue(ok);
        g_TC[pos+2].a0 =  edits[32]->GetFloatValue(ok);
        if(chk_dynamic3->GetCheckStatus())
            g_TC[pos+2].Dyna = 1;
        else
            g_TC[pos+2].Dyna = 0;
    }
}
void    CTCParam::OnButtonClick(skin_item_t* item)
{

    if(item->id == btn_mdy->GetId())
    {
       //CTajiDbMgr::Get().AddAlarmInfo(1,2,3,4,5,6);
    }
    else if(item->id == btn_save->GetId())
    {
        SaveTC(m_tc_id);
        CMainCtrl::Get().SaveTowerCraneInfo();
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
        if(m_tc_id >7)
          m_tc_id=1;
        if(m_tc_id==7)
        {
          Update(19,1);
          Update(20,2);
        }
        else
        {
          if(m_tc_id>0)
          {
            Update((m_tc_id-1)*3+1,1);
            Update((m_tc_id-1)*3+2,2);
            Update((m_tc_id-1)*3+3,3);
          }
        }

    } else if(item->id == btn_next->GetId())
    {
        m_tc_id--;
        if(m_tc_id == 7)
        {
          Update(19,1);
          Update(20,2);
        }
        else if(m_tc_id>0)
        {
          Update((m_tc_id-1)*3+1,1);
          Update((m_tc_id-1)*3+2,2);
          Update((m_tc_id-1)*3+3,3);
        }
        if(m_tc_id == 0)
          m_tc_id = 8;
    }

}
