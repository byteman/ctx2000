#ifndef PASSWORD_H
#define PASSWORD_H
#include "dt_core.h"

class PassWord : public CSkinForm
{
public:
    PassWord();
    virtual ~PassWord();
    int ShowBox(CSkinForm* parent,std::string strMsg,std::string strTitle,std::string strPassword="",DWORD type=MB_OK);
    virtual void   OnPaint(HWND hWnd);
    virtual void   OnClose();
    virtual void   OnShow();
    virtual void   OnButtonClick(skin_item_t* item);
private:
    std::string _msgStr;
    std::string _titleStr;
    std::string _passwordStr;
    CIcon       _edtBk;
    CSkinButton* _btnOk;
    CSkinButton* _btnClose;
    CEdit*       _edtPwd;
    CFont* Font24;
    bool   m_ok;
};

#endif // MSGBOX_H
