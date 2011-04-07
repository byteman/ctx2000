
#ifndef _WORKSITE_H
#define _WORKSITE_H

#include "dt_core.h"


class CWorkSite : public CSkinForm
{
public:
        CWorkSite();
        virtual ~CWorkSite();
	virtual void   OnPaint(HWND hWnd);
        virtual void   OnShow();
        virtual void   OnButtonClick(skin_item_t* item);
private:
        CEdit*      edt_leftdown_x;
        CEdit*      edt_range2;
        CEdit*      edt_range3;
        CSkinButton* btn_mdy;
        CSkinButton* btn_save;
        CSkinButton* btn_exit;
        CSkinButton* btn_ret;
        CIconGroup   _lables;
        CStatic*     lables[30];
         CEdit*      edits[30];

};

#endif


