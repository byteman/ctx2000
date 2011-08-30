#ifndef FORMCALIB_H
#define FORMCALIB_H
#include "dt_core.h"
#include "FastStatic.h"
#define BUTTON_RETURN_X           640
#define BUTTON_RETURN_Y           420
class CFormCalib:public CSkinForm
{
public:
    CFormCalib();
    ~CFormCalib();
    virtual void OnCreate();
    virtual void OnClose();
    virtual void OnTimer(int ID);
    virtual void OnPaint(HWND hWnd);
    virtual void OnShow();
    virtual void OnUserMsg(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
    virtual void OnCommCtrlNotify(HWND hwnd, int id, int nc);
    virtual void OnButtonClick(skin_item_t* item);
private:
    void calibrate_angle(int type);
    void calibrate_height(int type);
    void calibrate_car_dist(int type);
    void calibrate_weight(int type);
    bool calibrate_weight_next(int start_ad);
    bool calibrate_ok(int index,int start_ad,double bd_weight);
    bool calibrate_height_ok(int start_ad,double bd_height);
    bool calibrate_height_next(int start_ad);
    void calc_up_angle(int type);
    CSkinButton* _btns[20];
    CSkinButton* btn_ret;
    CSkinButton* btn_angle_zero;
    CEdit*       _edits[10];
    CStatic*     _lables[10];
    CFastStatic* _edtAD[5];
    CFastStatic* _edtLable[5];
};

#endif // FORMCALIB_H
