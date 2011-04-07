
/*
  File Name: CTemlateForm_Class.h

  Desc: class CTemlateForm_Class header file.

  Author: byteman  Date: 2009-11-23

  Version: 1.0

  Copyright: XXXXX

  Modified: Achilles Date: 2009-11-26
*/

#ifndef CUSERMANFORM_CLASS_H
#define CUSERMANFORM_CLASS_H

#include "SkinForm.h"
#include "dt_core.h"
class TUser;
#define USERMAN_SKIN_BTNS_NUM    5
class CLiJuManForm:public CSkinForm
{
public:

    CLiJuManForm();
    virtual ~ CLiJuManForm();
    virtual void OnShow();
    virtual void OnPaint(HWND hWnd);
    virtual void OnClose();
    virtual void OnButtonClick(skin_item_t * item);
    virtual void OnCommCtrlNotify(HWND hwnd, int id, int nc);
private:
    void InitListCol();
    void ReloadLijuItems();
    void RefreshList(std::string type, std::string len ,std::string beilv);
    CListView* _lvLiJu;
    CSkinButton*    _skinBtns[USERMAN_SKIN_BTNS_NUM];
    CIconGroup      _icons;
    CComBox         *cbx_type,*cbx_arm_len,*cbx_beilv;

};

#endif	//CUSERMANFORM_CLASS_H
