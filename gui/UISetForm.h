#ifndef PERIPHERALSETTINGFORM_H
#define PERIPHERALSETTINGFORM_H

#include "SkinForm.h"
#include "dt_core.h"

class CUISetForm : public CSkinForm
{
public:
    CUISetForm();

    virtual ~CUISetForm();

    virtual void OnShow();
    virtual void OnPaint(HWND hWnd);
    virtual void OnClose();
    virtual void OnButtonClick(skin_item_t* item);
    void savedata();
private:
    CSkinChkButton  *_skinChk[8];
    CSkinButton *_saveButton;
    CSkinButton *_exitButton;
    CIconGroup   _icons;
    CFont       *Font16;
    RECT        m_lable[8];
    int         m_cur_mode;

};

#endif // PERIPHERALSETTINGFORM_H