
/*
  File Name: CTemlateForm_Class.h

  Desc: class CTemlateForm_Class header file.

  Author: byteman  Date: 2009-11-23

  Version: 1.0

  Copyright: XXXXX

  Modified: Achilles Date: 2009-11-26
*/

#ifndef CLOGINFORM_CLASS_H
#define CLOGINFORM_CLASS_H

#include "SkinForm.h"
#include "dt_core.h"
#include "dataacquire.h"
class CLoginForm:public CSkinForm
{
  public:

	CLoginForm();
	virtual ~ CLoginForm();
	virtual void OnCreate();
	virtual void OnShow();
	virtual void OnTimer(int ID);
        virtual void OnKeyUp(int scancode, DWORD shiftstate);
	virtual void OnCommCtrlNotify(HWND hwnd, int id, int nc);
//	void OnCommand(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
	void OnUserMsg(HWND hWnd, int message, WPARAM wParam,
						   LPARAM lParam);
	virtual void OnPaint(HWND hWnd);
	virtual void OnClose();
	virtual void OnButtonClick(skin_item_t * item);
	virtual void OnLButtonDown(DWORD keystatus, int x, int y);
        void handle1(ADNotification* pNf);

  private:
  	bool CheckPWD(HWND hWnd);
        void InitIME();
	int m_msg_delay;

        CIconGroup _lables;
        CIconGroup  _msg;
        CEdit* _edtUser;
        CEdit *_edtPwd;
        CSkinButton* _btnLogin;
        int repeat;

};

#endif
