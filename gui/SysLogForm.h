/*
  File Name: CTemlateForm_Class.h

  Desc: class CTemlateForm_Class header file.

  Author: byteman  Date: 2009-11-23

  Version: 1.0

  Copyright: XXXXX

  Modified: Achilles Date: 2009-12-14
*/

#ifndef SYSLOGFORM_CLASS_H
#define SYSLOGFORM_CLASS_H

#include "SkinForm.h"
#include "dt_core.h"
#include "tajidbmgr.h"
#define ALARMINFO_SKIN_BTNS_NUM    6
class CSysLogForm : public CSkinForm
{
public: 
	
        CSysLogForm();
        virtual ~CSysLogForm();
        virtual void   OnShow();
	virtual void   OnPaint(HWND hWnd);
	virtual void   OnButtonClick(skin_item_t* item);
private:
	void InitListCol();
	void RefreshList(int tailindex);
        void add_alarminfo_item (StringList  &alarminfoItems, TSysLog& AlarmInfo);
        //void add_alarminfo_item (StringList &alarminfoItems, TAlarmInfo* p_AlarmInfo);
private:
 	int m_tailindex;
        CListView*      _lvAlarmInfo;
        CSkinButton*    _skinBtns[ALARMINFO_SKIN_BTNS_NUM];
        CIconGroup      _icons;
        int             m_colum_num;
        int             start;
        int             m_type; // 0:报警信息  1:起吊记录
        void            ChangeType(int type);
};

#endif

