
#ifndef _TCPARAM_H
#define _TCPARAM_H

#include "dt_core.h"


class CTCParam : public CSkinForm
{
public:
        CTCParam();
        virtual ~CTCParam();
        virtual void  OnCommCtrlNotify(HWND hwnd, int id, int nc);
	virtual void   OnPaint(HWND hWnd);
        virtual void   OnShow();
        virtual void   OnButtonClick(skin_item_t* item);
        void    Update(int id,int where);
        void    SaveTC(int id);
private:
        CEdit*      edt_leftdown_x;
        CEdit*      edt_range2;
        CEdit*      edt_range3;
        CSkinButton* btn_mdy;
        CSkinButton* btn_save;
        CSkinButton* btn_exit;
        CSkinButton* btn_ret;
        CSkinButton* btn_next;
        CSkinButton* btn_prev;
        CIconGroup   _lables;
        CStatic*     lables[30];
        CEdit*      edits[40];
        CSkinChkButton* chk_dynamic1;
        CSkinChkButton* chk_dynamic2;
        CSkinChkButton* chk_dynamic3;
        int         m_tc_id;

};

#endif


