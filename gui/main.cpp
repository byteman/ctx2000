#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "torqueForm.h"
void GUIAPI InitMainUI()
{
    HWND hMainWnd;

    CTorQueForm form;
    form.CreateForm(HWND_DESKTOP);

    MiniGUIExtCleanUp();

    exit(0);
}

int MiniGUIMain (int argc, const char* argv[])
{
    if (!InitMiniGUIExt())
    {
        fprintf ( stderr, "Can't InitMiniGUIExt!\n" );
        return 2;
    }



    InitMainUI ();

    fprintf ( stderr, "guiApp Exit!\n" );
    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif



