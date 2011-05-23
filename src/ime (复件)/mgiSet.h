#ifndef __MGISET_H__
#define __MGISET_H__

#include "dt_core.h"
#include "mgime.h"
#include "keyboard.h"
#include "mgphone.h"
#include "hwinput.h"


#define INPUT_NUM   1
#define INPUT_ABC   2
#define INPUT_abc   3
#define INPUT_EN    100
#define INPUT_PY    101


typedef void (*Method_Switch_Notify)(int method);

class mgiSet {
public:
    mgiSet(int left=0 ,int top=270 ,int right=240,int bottom=320);
    ~mgiSet();
    void switchinput(int input = 0);
    void Register_Callback_Fun(Method_Switch_Notify func);
    void move(int x,int y);
    
private:
    GHANDLE ime_handle;
    HWND sk_ime_hwnd;
    MGI_SOFTKEYBOARD_INFO sk_info;

    HWND Create_Phone_IME(RECT *rect);
};

#endif
