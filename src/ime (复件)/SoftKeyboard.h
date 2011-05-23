#ifndef __SOFTKEYBOARD_H
#define __SOFTKEYBOARD_H

#include "mgiSet.h"

#define   T9_NUM    0//INPUT_NUM
#define   T9_ABC    3//INPUT_ABC
#define   T9_abc    4//INPUT_abc
#define   T9_EN     2//INPUT_EN
#define   T9_PY     1//INPUT_PY

#define   SOFTKEYBOARD_X  450
#define   SOFTKEYBOARD_Y  220
class SoftKeyboardForm;
class SoftKeyboard {
public:
    SoftKeyboard(int left   = SOFTKEYBOARD_X ,
                 int top    = SOFTKEYBOARD_Y);
    ~SoftKeyboard();
    void switchinput(int input = INPUT_NUM);
    void T9_Show(bool show);
    void move(int x,int y);
private:
    mgiSet*           _pmgi;
    HWND              T9key;
    SoftKeyboardForm* _skForm;

};

#endif
