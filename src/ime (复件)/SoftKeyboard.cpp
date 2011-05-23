#include "SoftKeyboard.h"
#include "SoftKeyboardForm.h"

SoftKeyboard::SoftKeyboard(int left ,int top)
{
    //初始化内部变量
    SoftKeyboardForm::Init();
    _skForm = new SoftKeyboardForm();
    //在另外一个线程中创建一个topmost窗体
    T9key   = HWND_INVALID;
    T9key   = _skForm->CreateFormEx(HWND_DESKTOP);
    if(T9key == HWND_INVALID)
    {
        fprintf(stderr, "Create SoftKeyForm Failed\n");
        return;
    }
    //SetWindowAdditionalData2(T9key,0xaa55);
    ShowWindow(T9key, SW_HIDE);
    _pmgi = new mgiSet(left,top-50,left+240,top);
    _pmgi->switchinput(T9_NUM);
    _skForm->SetInputMethod(T9_NUM);
}

SoftKeyboard::~SoftKeyboard()
{
    SoftKeyboardForm::Close();
    delete _pmgi;
    if(_skForm)
        delete _skForm;

}

void
SoftKeyboard::switchinput(int input)
{
    _pmgi->switchinput(input);
}

void 
SoftKeyboard::T9_Show(bool show)
{
    if (show) {
        fprintf(stderr,"show t9key=%x\n",T9key);
        ShowWindow(T9key, SW_SHOWNORMAL);
    } else {
        _pmgi->switchinput(T9_NUM);
        ShowWindow(T9key, SW_HIDE);
    }
}

void 
SoftKeyboard::move(int x,int y)
{
    _pmgi->move(x, y);
}

    
