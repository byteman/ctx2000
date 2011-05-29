#ifndef FORMCALIB_H
#define FORMCALIB_H
#include "dt_core.h"
#include <yateclass.h>
#include <yatengine.h>
using namespace TelEngine;
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
    void calc_up_angle(int type);
    CSkinButton* _btns[20];
    CSkinButton* btn_ret;
    CEdit*       _edits[10];
    CStatic*     _lables[10];
    MessageHandler* msgHandler;
};

#endif // FORMCALIB_H
