#ifndef FORMCALIBDIST_H
#define FORMCALIBDIST_H
#include "dt_core.h"

class CFormCalibDist:public CSkinForm
{
public:
    CFormCalibDist();
    ~CFormCalibDist();
    virtual void OnCreate();
    virtual void OnTimer(int ID);
    virtual void OnPaint(HWND hWnd);
    virtual void OnShow();
    virtual void OnButtonClick(skin_item_t* item);
private:
    CSkinButton* _btns[10];
    CEdit*       _edits[10];
    CStatic*     _lables[10];
};

#endif // FORMCALIB_H
