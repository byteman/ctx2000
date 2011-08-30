/*
  File Name: CTemlateForm_Class.h

  Desc: class CTemlateForm_Class header file.

  Author: byteman  Date: 2009-11-23

  Version: 1.0

  Copyright: XXXXX

  Modified: Achilles Date: 2009-12-14
*/

#ifndef IPSET_CLASS_H
#define IPSET_CLASS_H

#include "SkinForm.h"
#include "dt_core.h"
#define IPSET_EDIT_NUM 4 //2
#define IPSET_BTN_NUM  3
#define IPSET_CHECK_NUM 1//2
#define IPSET_IPEDIT_NUM 6//10 del gprs
class CIPset : public CSkinForm
{
public:

    CIPset();
    virtual ~CIPset();
    virtual void   OnCreate();
    virtual void   OnTimer(int ID);
    virtual void   OnPaint(HWND hWnd);
    virtual void   OnClose();
    virtual void   OnShow();
    virtual void   OnButtonClick(skin_item_t* item);
    virtual void   OnCommCtrlNotify(HWND hwnd, int id, int nc);

private:
    /*	int addr1,addr2,addr3,addr4;
    	int mask1,mask2,mask3,mask4;
    	int gate1,gate2,gate3,gate4;
    	int dns1,dns2,dns3,dns4;
    	BOOL m_bChanged;*/
    int CheckIPset();
    void PromoteErrMsg(int type);
    int m_msg_delay;
    CEdit* _edit[IPSET_EDIT_NUM];
    CSkinButton* _skinBtns[IPSET_BTN_NUM];
    CSkinChkButton* _skinChks[IPSET_CHECK_NUM];
    CIpAddr* _ipedit[IPSET_IPEDIT_NUM];
    CIconGroup _lables;
    CIconGroup _msgs;
};

#endif //IPSET_CLASS_H

