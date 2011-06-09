#include "guinotifyer.h"

TGuiNotifyer::TGuiNotifyer()
{

}
void TGuiNotifyer::RegisterWindow(HWND hwnd)
{
    m_hwnd = hwnd;
}
