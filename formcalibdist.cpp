#include "formcalibdist.h"
#include "formcalibdistitem.h"
#include "BmpRes.h"
static const char* mmenu_bmps[] = {
     PCOMM_BACKGROUND,
     PSETZERO_BTN,
     PCALB_BTN,
     PCOMM_SAVE_BTN,
     PCOMM_CLOSE_BTN,
     PCOMM_RET_BTN,
     PMODIFY_BTN,  //new add button for line k
     PREAD_BTN,  //new add button for read line k
};
static COMM_CTRL_DESC CommCtrls[] =
{
    EDIT_CUR_AD,
    EDIT_START_X,
    EDIT_START_Y,
    EDIT_END_X,
    EDIT_END_Y,
    EDIT_START_AD,
    EDIT_END_AD,

};
static COMM_CTRL_DESC StaticCtrls[] =
{

    STATIC_CUR_AD,
    STATIC_START_X,
    STATIC_START_Y,
    STATIC_END_X,
    STATIC_END_Y,
    STATIC_START_AD,
    STATIC_END_AD,
};
static SKIN_CTRL_DESC SkinCtrls[] = {
    BUTTON_CABZERO,
    BUTTON_CABK,
    BUTTON_SAVE,
    BUTTON_EXIT,
    BUTTON_RET_CALIB,
    BUTTON_LINEK,
    BUTTON_READ_LINEK
};
CFormCalibDist::CFormCalibDist()
{
    if(!LoadRes(&mmenu_bmps[0],ARRAY_SIZE(mmenu_bmps,char*)))
    {
        printf("Can't loadres\n");
        exit(0);
    }
    for(int i = 0 ;i < (sizeof(SkinCtrls)/sizeof(SKIN_CTRL_DESC));i++)
    {
        _btns[i] = new CSkinButton(&SkinCtrls[i],this);
    }
    for(int i = 0 ;i < (sizeof(CommCtrls)/sizeof(COMM_CTRL_DESC));i++)
    {
        _edits[i] = new CEdit(&CommCtrls[i],this);
    }

    for(int i = 0 ;i < (sizeof(StaticCtrls)/sizeof(COMM_CTRL_DESC));i++)
    {
        _lables[i] = new CStatic(&StaticCtrls[i],this);
    }



    InitSkinHeader("formcalib");
}

CFormCalibDist::~CFormCalibDist()
{

}

void CFormCalibDist::OnCreate()
{

}
void CFormCalibDist::OnTimer(int ID)
{

}
void CFormCalibDist::OnPaint(HWND hWnd)
{

}
void CFormCalibDist::OnShow()
{

}
void CFormCalibDist::OnButtonClick(skin_item_t* item)
{

}
