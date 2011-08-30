#ifndef FORMEXTCFG_H
#define FORMEXTCFG_H
#include "SkinForm.h"
#include "dt_core.h"
class FormExtCfg:public CSkinForm
{
public:
    FormExtCfg();
    ~FormExtCfg();
    virtual void   OnShow();
    virtual void   OnClose();
    virtual void   OnPaint(HWND hWnd);
    virtual void   OnButtonClick(skin_item_t *item);
    virtual void   OnCommCtrlNotify(HWND hwnd, int id, int nc);
    bool SaveCfg();
private:
    CSkinButton* _btnSave;
    CSkinButton* _btnRet;
    CSkinButton* _btnTest;
    CSkinButton* _btnQuit;
    CComBox* _cbx[16];
    CIcon      _nums;
    CIcon      _inText;
    CIcon      _outText;
    CIcon      _inStat;
    CIconGroup _icons;
};

#endif // FORMEXTCFG_H
