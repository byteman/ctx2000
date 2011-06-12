// MainMenu.cpp: implementation of the CMainMenu class.
//
//////////////////////////////////////////////////////////////////////
#include "SysSetItem.h"
#include "SysSet.h"
#include "worksite.h"
#include "formcalib.h"
#include "AlarmInfoManForm.h"
#include "tcparam.h"
#include "divparam.h"
#include "BmpRes.h"
#include "iniFile.h"
#include "comdata.h"
#define EDIT_ANGLE   {20,250,160,30,"20.8"}
#define EDIT_DIST    {20,320,160,30,"12.4"}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static const char* mmenu_bmps[] = {

    "ctx2000/mainbk.png",
    PZONE,
    PTOWERSET,
    PWORKSITE,
    PDIVDING,
    PHISTORY,
    PPARAMSET,
    PCOMM_CLOSE_BTN,
    PCOMM_CHECK_BTN
};
static SKIN_BUTTON_DESC SkinBtns[] = {

    SKIN_BUTTON_ZONE,
    SKIN_BUTTON_TOWER,
    SKIN_BUTTON_WORKSITE,
    SKIN_BUTTON_DIV,
    SKIN_BUTTON_HISTORY,
    SKIN_BUTTON_PARAM,
    {7,738,8},
    {8,246,28},
    {8,558,28},
};
static COMM_CTRL_DESC commctrls[] = {
    EDIT_ANGLE,
    EDIT_DIST
};

CSysSet::CSysSet()
{

    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
    for (int i = 0; i < SYS_SKIN_BTN_NUM; i++)
    {
        _skinBtns[i] = new CSkinButton(&SkinBtns[i],this);
    }

    _chkBtns[0] = new CSkinChkButton(&SkinBtns[7],this);
    _chkBtns[1] = new CSkinChkButton(&SkinBtns[8],this);
    TIniFile cfg("ctx2000.ini");
    m_cur_mode =cfg.ReadInteger("display","mainmenu",1);


    InitSkinHeader("MainMenu");

}

CSysSet::~CSysSet()
{

}
void CSysSet::OnShow()
{
    if(m_cur_mode == 1)
    {
        _chkBtns[0]->SetCheckStatus(true);
        _chkBtns[1]->SetCheckStatus(false);
    }else if(m_cur_mode == 2)
    {
        _chkBtns[1]->SetCheckStatus(true);
        _chkBtns[0]->SetCheckStatus(false);
    }
}
void CSysSet::OnClose()
{

    if(_chkBtns[0]->GetCheckStatus()){

        if(m_cur_mode==1)
            return;
        else
            m_cur_mode = 1;
    }

    if(_chkBtns[1]->GetCheckStatus()){

        if(m_cur_mode==2)
            return;
         else
            m_cur_mode = 2;
    }

    TIniFile cfg("ctx2000.ini");
    cfg.WriteInteger("display","mainmenu",m_cur_mode);


}
void CSysSet::OnCreate()
{

}

void CSysSet::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);

    EndPaint(hWnd, hdc);
}
void CSysSet::OnButtonClick(skin_item_t* item)
{
    if(item->id == _skinBtns[0]->GetId()){
        CDivParam form;
        form.CreateForm(m_hWnd);

    }else if(item->id == _skinBtns[1]->GetId()){

        CTCParam param;
        param.CreateForm(m_hWnd);
    }else if(item->id == _skinBtns[2]->GetId()){
        CWorkSite ws;
        ws.CreateForm(m_hWnd);
    }else if(item->id == _skinBtns[3]->GetId()){

    }else if(item->id == _skinBtns[4]->GetId()){
        CAlarmInfoManForm alarm;
        alarm.CreateForm(m_hWnd);

    }else if(item->id == _skinBtns[5]->GetId()){
        CFormCalib cali;
        cali.CreateForm(m_hWnd);
    }else if(item->id == _skinBtns[6]->GetId()){
        Close();
    }else if(item->id == _chkBtns[0]->GetId()){

        bool ok = _chkBtns[0]->GetCheckStatus();

        if(ok){
            _chkBtns[1]->SetCheckStatus(false);
        }else{
            _chkBtns[1]->SetCheckStatus(true);
        }
        //fprintf(stderr,"7 = %d\n",ok);
    }else if(item->id == _chkBtns[1]->GetId()){
        bool ok = _chkBtns[1]->GetCheckStatus();
        if(ok){
            _chkBtns[0]->SetCheckStatus(false);
        }else{
            _chkBtns[0]->SetCheckStatus(true);
        }
        //fprintf(stderr,"8 = %d\n",ok);
    }

}
