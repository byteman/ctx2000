#ifndef CALIBOX_H
#define CALIBOX_H
#include "dt_core.h"

class CaliBox : public CSkinForm
{
public:
    CaliBox(int type=1);
    virtual ~CaliBox();
    int ShowBox(CSkinForm* parent,std::string strMsg,std::string strTitle,DWORD type=MB_OK);
    virtual void   OnPaint(HWND hWnd);
    virtual void   OnClose();
    virtual void   OnShow();
    virtual void   OnButtonClick(skin_item_t* item);
    virtual void   OnCommCtrlNotify(HWND hwnd, int id, int nc);
    bool    check_valid(double &value);
    std::string  m_input_value;
    double       m_double_input_value;
private:
    std::string _msgStr;
    std::string _titleStr;
    CSkinButton* _btnOk,*_btnNext, *_btnCancel;
    CSkinButton* _btnClose;
    CEdit*       _edtInput;
    CIcon        iconBk;
    int          m_ret;
    int          m_type;
    CFont* Font24;
};

#endif // MSGBOX_H
