// InitForm.h: interface for the CInitForm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITFORM_H__1A7815C9_CBC6_4F0F_AAB3_17DCBF1F99FE__INCLUDED_)
#define AFX_INITFORM_H__1A7815C9_CBC6_4F0F_AAB3_17DCBF1F99FE__INCLUDED_

#include "SkinForm.h"
#include "dt_core.h"
struct INFO_ITEM
{
    std::string text;
    int  err;
    RECT rt;
};
typedef std::vector<INFO_ITEM> TRECTVECT;
class CInitForm : public CSkinForm
{
public:

    CInitForm();
    virtual ~CInitForm();
    virtual void   OnCreate();
    //virtual  void   OnButtonClick(skin_item_t* p_item);
    virtual void   OnTimer(int ID);
    virtual void   OnUserMsg(HWND hWnd, int message,WPARAM wParam, LPARAM lParam);
    virtual void   OnPaint(HWND hWnd);
    virtual void   OnKeyUp(int scancode, DWORD shiftstate);
    virtual void   OnClose();
    virtual  void   OnShow();
    void    device_check();
    void    check_kernel();
    void    addItem(std::string text,int height,int err=0);
    void    StartCtx2000();
private:
    TRECTVECT items_vct;
    RECT header_rect;
    BITMAP m_bkBmp;
    CListBox *lbx_info;
    CFont* load_font;
    bool  _finished;

};

#endif // !defined(AFX_INITFORM_H__1A7815C9_CBC6_4F0F_AAB3_17DCBF1F99FE__INCLUDED_)
