#ifndef TORQUE_FORM_H
#define TORQUE_FORM_H

#include "SkinForm.h"
#include "dt_core.h"
#include "usbstormanager.h"
#define USERMAN_SKIN_BTNS_NUM    6
class CTorQueForm:public CSkinForm,public IUSBStorNotify
{
public:

    CTorQueForm();
    virtual ~ CTorQueForm();
    virtual void OnShow();
    virtual void OnPaint(HWND hWnd);
    virtual void OnClose();
    virtual void OnButtonClick(skin_item_t * item);
    virtual void OnCommCtrlNotify(HWND hwnd, int id, int nc);
    bool UsbStorNotify(U_STATUS status,std::string mountDir);
private:
    bool checkValid();
    bool DoIt(int type,std::string dist,std::string wet);
    void InitListCol();
    void ReloadTorqueItems(std::string type, std::string armLen, std::string rate,bool clear=false);
    bool loadDB(std::string cur_type, std::string cur_armLen, std::string cur_rate);
    void RefreshList(std::string type, std::string len ,std::string beilv);
    CTreeView*      tv;
    CListView*      lv;
    CSkinButton*    _skinBtns[USERMAN_SKIN_BTNS_NUM];
    CIconGroup      _icons;
    CComBox         *cbx_type,*cbx_arm_len,*cbx_beilv;
    CFont*          Font16;
    RECT            m_lable[3];
    bool            m_change_fall;
    std::string     m_cur_tctype,m_cur_armlen,m_cur_rate;
    GHANDLE         m_sel_hwnd;
    CIcon           m_usb;

};

#endif	//CUSERMANFORM_CLASS_H
