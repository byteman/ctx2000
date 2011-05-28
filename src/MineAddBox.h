#ifndef CMINEADDBOX_CLASS_H
#define CMINEADDBOX_CLASS_H

#include "SkinForm.h"
#include "dt_core.h"
#define MINEADD_EDIT_NUM 2
#define MINEADD_BTN_NUM  3
class CMineAddBox : public CSkinForm
{
public:

    CMineAddBox();
    virtual ~CMineAddBox();
    virtual void   OnCreate();
    virtual void   OnTimer(int ID);
    virtual void   OnPaint(HWND hWnd);
    virtual void   OnClose();
    virtual void   OnShow();
    virtual void   OnButtonClick(skin_item_t* item);
    void  OnCommCtrlNotify(HWND hwnd, int id, int nc);
    std::string m_arm_len;
    std::string m_weight;
    bool    m_bOk;
private:
    int AddMine();
    void PromoteErrMsg(int type);
    int m_msg_delay;
    CEdit* _edit[MINEADD_EDIT_NUM];
    CSkinButton* _skinBtns[MINEADD_BTN_NUM];
    CIconGroup _lables;

    CIconGroup _msgs;

};

#endif

