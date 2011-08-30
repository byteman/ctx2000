#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <cassert>
#include <Poco/Format.h>
#include <Poco/Bugcheck.h>
#include <Poco/ThreadPool.h>
#include <Poco/Thread.h>
#include <Poco/SingletonHolder.h>
#include <Poco/AutoPtr.h>
#include "dt_core.h"
#include "InitForm.h"
#include "mainctrl.h"
#include "MainMenu.h"
#include "SingleDynForm.h"
#include "SingleFlatForm.h"
#include "comdata.h"
#include "beeper.h"
using Poco::Thread;
using Poco::Event;
using Poco::SingletonHolder;

static std::string gAppName="";

void GUIAPI InitMainUI()
{

    try{
        {
            //CInitForm init;
            //init.CreateForm(HWND_DESKTOP);
        }

        switch(gMainMenuIndex)
        {
            case 1:
            {
                CMainMenu mainmenu;
                mainmenu.CreateForm( HWND_DESKTOP );
                break;
            }
            case 2:
            {
                if(g_TC[g_local_id].Dyna)
                {
                    CSingleFlatForm form;
                    form.CreateForm( HWND_DESKTOP );
                }else{
                    CSingleDynForm form;
                    form.CreateForm( HWND_DESKTOP );
                }

                break;
            }
            default:
            {
                CMainMenu mainmenu;
                mainmenu.CreateForm( HWND_DESKTOP );
                break;
            }
        }


    }catch(Poco::Exception& e){
        std::cerr << "Sys Exception: " <<e.displayText () << std::endl;
    }catch(...)
    {
        std::cerr << "Unkown Exception\n";
    }
    fprintf(stderr,"Stop App\n");
    CMainCtrl::Get().Stop();

    MiniGUIExtCleanUp();

    std::cerr << "Exit guiApp\n";

}

int MiniGUIMain (int argc, const char* argv[])
{

    gAppName=argv[0];
    argc    = argc;

    fprintf(stderr,"%s build @ %s %s",argv[0],__DATE__,__TIME__);
#if 0
    if( !CtxEngine::Get().start( 1000, argc, argv ) )
    {
        std::cerr << "CtxEngine start failed\n";
        return 1;
    }
#endif
    if (!InitMiniGUIExt()) {
        fprintf ( stderr, "Can't InitMiniGUIExt!\n" );
        return 2;
    }

    //启动主控制模块

    if( ! CMainCtrl::Get().Start())
    {
        fprintf(stderr,"MainCtrl Start Failed\n");

        for(size_t i = 0 ; i < CMainCtrl::Get().m_errList.size(); i++)
        {
            std::cerr << CMainCtrl::Get().m_errList.at(i) << std::endl;
        }
        for(size_t i = 0 ; i < CMainCtrl::Get().m_warnList.size(); i++)
        {
            std::cerr << CMainCtrl::Get().m_warnList.at(i) << std::endl;
        }
        CBeeper::get().BeepMs(200,100000);
        Poco::Thread::sleep(400);
        CBeeper::get().BeepMs(200,100000);
        Poco::Thread::sleep(400);
        CBeeper::get().BeepMs(200,100000);
    }

    //启动主界面模块
    InitMainUI ();
	
    fprintf ( stderr, "guiApp Exit!\n" );
    exit(0);
    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif



