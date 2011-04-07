#ifndef IMEWND_H
#define IMEWND_H
#include "SkinForm.h"
#include "dt_core.h"
class CIMEWnd;
typedef struct _CONTAINER_INFO
{
    sem_t wait;
    HWND hwnd;
    CIMEWnd* arg;
} CONTAINER_INFO;
class CIMEWnd
{
public:
    CIMEWnd();
    int  StartIME();
    int  InitIME();
    bool MoveWindowPos(int x, int y,BOOL fPaint=TRUE);
    int  MsgProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam);
    HWND m_hWnd;
    RECT imeRect;
    RECT subimeRect;
    RECT refRect;
private:
    int Key_Check(int x, int y);
    CIcon icon;
    CIcon logo;

};

#endif // IMEWND_H
