/*
  File Name: CTemlateForm_Class.h

  Desc: class CTemlateForm_Class header file.

  Author: byteman  Date: 2009-11-23

  Version: 1.0

  Copyright: XXXXX

  Modified: Achilles Date: 2009-12-14
*/

#ifndef ALARMINFOMANFORM_CLASS_H
#define ALARMINFOMANFORM_CLASS_H

#include "SkinForm.h"
#include "dt_core.h"
#include "tajidbmgr.h"
#include "usbstormanager.h"
#define ALARMINFO_SKIN_BTNS_NUM    6
class CAlarmInfoManForm : public CSkinForm,public IUSBStorNotify
{
public: 
	
    CAlarmInfoManForm();
    virtual ~CAlarmInfoManForm();
    virtual void   OnShow();
    virtual void   OnPaint(HWND hWnd);
    virtual void   OnClose ();
    virtual void   OnButtonClick(skin_item_t* item);
    bool UsbStorNotify(U_STATUS status,std::string mountDir);
private:
    void InitListCol();
    void RefreshList(int tailindex);
    void add_alarminfo_item (StringList  &alarminfoItems, THistoy& AlarmInfo);
    void add_weightinfo_item (StringList &weightinfoItems,TWeightHistoy& WeightInfo);
private:
 	int m_tailindex;
    CListView*      _lvAlarmInfo;
    CSkinButton*    _skinBtns[ALARMINFO_SKIN_BTNS_NUM];
    CIconGroup      _icons;
    int             m_colum_num;
    int             start;
    int             m_type; // 0:报警信息  1:起吊记录
    void            ChangeType(int type);
    CIcon           m_usb;
};

#endif

