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
#include <yatengine.h>
#include <yateclass.h>
#include <yatephone.h>
#include "dt_core.h"
#include "LoginForm.h"
#include "dataacquire.h"
#include "mainctrl.h"
#include "MainMenu.h"
#include "SingleTaji1.h"
#include "comdata.h"
#include "SinalTaji2.h""
using Poco::Thread;
using Poco::Event;
using Poco::SingletonHolder;
using namespace TelEngine;
static std::string gAppName="";
/*
CIconGroup grp;

static const char* preLoadBmp[] =
{
    "ctx2000/A.png",
    "ctx2000/angle.jpg",

};
*/
class CtxEngine:public Poco::Runnable{
public:
    static CtxEngine& Get()
    {
        static SingletonHolder<CtxEngine> sh;
        return *sh.get();
    }
    CtxEngine()
    {

    }
    void run()
    {
        m_evt.set();
        TelEngine::Engine::main(m_argc,m_argv,NULL);
    }
    bool start( int timeout,int argc, const char *argv[] )
    {
        m_argc = argc;
        m_argv = argv;
        m_thread.start(*this);
        return m_evt.tryWait(timeout);
    }
private:
    Poco::Event  m_evt;
    Poco::Thread m_thread;
    int          m_argc;
    const char** m_argv;
};

void GUIAPI InitMainUI()
{

    try{

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
                    CSingleTaji2 mainmenu;
                    mainmenu.CreateForm( HWND_DESKTOP );
                }else{
                    CSingleTaji1 mainmenu;
                    mainmenu.CreateForm( HWND_DESKTOP );
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
       // return 3;
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



