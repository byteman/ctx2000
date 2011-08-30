#ifndef MSGBOX_H
#define MSGBOX_H
#include "dt_core.h"

class MsgBox : public CSkinForm
{
public:
    MsgBox();
    virtual ~MsgBox();
    int ShowBox(CSkinForm* parent,std::string strMsg,std::string strTitle,DWORD type=MB_OK);
    virtual void   OnPaint(HWND hWnd);
    virtual void   OnClose();
    virtual void   OnShow();
    virtual void   OnButtonClick(skin_item_t* item);
private:
    std::string _msgStr;
    std::string _titleStr;
    CSkinButton* _btnOk;
    CSkinButton* _btnClose;
    CFont* Font24;
};

#endif // MSGBOX_H
