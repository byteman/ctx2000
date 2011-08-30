#ifndef SYSCFGFORM_H
#define SYSCFGFORM_H

#include "SkinForm.h"
#include "dt_core.h"

class CSysCfgForm : public CSkinForm
{
public:
    CSysCfgForm();
    virtual ~CSysCfgForm();
    virtual void OnPaint(HWND hWnd);
    virtual void OnShow();
    virtual void OnButtonClick(skin_item_t* item);
private:
    void calib_touch_screen();
    CSkinButton* _skinBtns[6];
    CIconGroup _menus;
};

#endif
