// InitForm.cpp: implementation of the CInitForm class.
//
//////////////////////////////////////////////////////////////////////
#include "InitForm.h"
#include "dt_core.h"
#include "BmpRes.h"
#include "iniFile.h"
#include <Poco/File.h>
#include <Poco/Format.h>
#include <sys/vfs.h>
#include <Poco/Glob.h>
#include <Poco/FileStream.h>
#include "mainctrl.h"
#include "beeper.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define LBX_INFO     {100,50,600,400,"0.0.0.0"}
static COMM_CTRL_DESC commctrls[] =
{
    LBX_INFO
};
static const char* mmenu_bmps[] =
{
    PCOMM_START_PIC
};
static const char* preLoadBmp[] =
{
    PCOMM_LIST_BKGROUND,
    PCOMM_PREVPAGE_BTN,
    PCOMM_NEXTPAGE_BTN,
    PCOMM_RET_BTN,
    PCOMM_CLOSE_BTN,
    PCOMM_CHECK_BTN,
    PMAINCFG,
    PBYPASS,
    PZONE,
    PTOWERSET,
    PWORKSITE,
    PDIVDING,
    PHISTORY,
    PPARAMSET,
    "ctx2000/pingbibk.png",
    "ctx2000/single_dy.png",
    "ctx2000/btn_alarm.png",
    "ctx2000/btn_weight.png",
    "ctx2000/height.jpg",
    "ctx2000/up_angle.jpg",
    "ctx2000/speed.jpg",
    "ctx2000/edt_bk.jpg",
    "ctx2000/msgboxbj.png",
    "ctx2000/ok_btn.png",
    "ctx2000/close_btn.png",
    "ctx2000/mainbk.png"
};

CInitForm::CInitForm()
{
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        //exit(0);
    }

    InitSkinHeader("CInitForm");

    header_rect.bottom= 50;
    load_font = CFontMgr::Get().GetFont("song",24);
    _finished =true;

}

CInitForm::~CInitForm()
{

}

void   CInitForm::OnCreate()
{

}
void   CInitForm::OnUserMsg(HWND hWnd, int message,WPARAM wParam, LPARAM lParam)
{

}
void   CInitForm::OnClose()
{
}
#define INFO_LEFT  10
#define INFO_RIGHT INFO_LEFT+500

void    CInitForm::addItem(std::string text,int height,int err)
{
    INFO_ITEM it;

    it.rt.left = INFO_LEFT;
    it.rt.top  = header_rect.bottom+2;
    it.rt.right = INFO_RIGHT;
    it.rt.bottom= it.rt.top+height;
    it.text = Poco::format("%-30s",text);
    it.err = err;
    header_rect.bottom = it.rt.bottom;
    if(err)
        _finished=false;
    items_vct.push_back(it);
}
void   CInitForm::OnPaint(HWND hWnd)
{
    HDC hdc=BeginPaint(hWnd);

    for(size_t i = 0 ; i < items_vct.size(); i++)
    {
        INFO_ITEM &it = items_vct.at(i);
        if(it.err)
            load_font->font_color=RGB2Pixel(HDC_SCREEN,255,0,0);
        else
            load_font->font_color=RGB2Pixel(HDC_SCREEN,0,255,0);
        DrawMyText(hdc,load_font,&it.rt,it.text, DT_LEFT|DT_TOP);
    }
    EndPaint(hWnd,hdc);
}

bool check_file(std::string file)
{
    Poco::File fd(file);

    return fd.exists();
}
unsigned long long  GetFreeDisk()
{
    struct statfs diskInfo;
    statfs("/",&diskInfo);
    unsigned long long totalBlocks = diskInfo.f_bsize;
    unsigned long long freeDisk = diskInfo.f_bfree*totalBlocks;
    return freeDisk>>10;
}
bool get_file_content(std::string path, std::string &context)
{
    Poco::FileInputStream fd(path);
    string szLine = "";
    context.clear();
    while(getline(fd,szLine))
    {
        context += szLine;
    }

    return true;
}

void    CInitForm::check_kernel()
{

    std::string context="";

    get_file_content("/proc/version",context);
    Poco::Glob g2("*Tue Dec 14 16:57:21 CST 2010*");
    //std::cerr << context << std::endl;
    if(g2.match(context))
    {
#ifdef ENGLISH_VER
        addItem("Kernel version=>Correct",24);
#else
        addItem("内核版本=>正确",24);
#endif
    }
    else
    {
#ifdef ENGLISH_VER
        addItem("Kernel version========>Incorrect",24,1);
#else
        addItem("内核版本========>不正确",24,1);
#endif
    }

}
void    CInitForm::device_check()
{

    if(check_file("/dev/rtc0"))
#ifdef ENGLISH_VER
        addItem("Checking RTC=>Successful",24);
#else
        addItem("RTC时钟检测中=>成功",24);
#endif
    else
#ifdef ENGLISH_VER
        addItem("Checking RTC=======>Fail",24,1);
#else
        addItem("RTC时钟检测中=======>失败",24,1);
#endif

    if(check_file("/sys/bus/i2c/devices/1-0020/input0"))
#ifdef ENGLISH_VER
        addItem("Detecting the expanded IO peripheral=>Successful",24);
#else
        addItem("外部扩展IO检测中=>成功",24);
#endif
    else
#ifdef ENGLISH_VER
        addItem("Detecting the expanded IO peripheral=======>Fail",24,1);
#else
        addItem("外部扩展IO检测中=======>失败",24,1);
#endif

    if(check_file("/dev/snd/pcmC0D0c"))
#ifdef ENGLISH_VER
        addItem("Detecting audio device=>Successful",24);
#else
        addItem("音频设备检测中=>成功",24);
#endif
    else
#ifdef ENGLISH_VER
        addItem("Detecting audio device=======>Fail",24,1);
#else
        addItem("音频设备检测中=======>失败",24,1);
#endif
    double per=(double)(GetFreeDisk())/(124*1024)*100;
#ifdef ENGLISH_VER
    addItem(Poco::format("Checking space=======>%0.1f",per)+"%",24);
#else
    addItem(Poco::format("检测剩余存储空间=======>%0.1f",per)+"%",24);
#endif
    char buf[128] = {0,};
    snprintf(buf,128,"ctx2000编译时间: %s %s",__DATE__,__TIME__);
    addItem(buf,24);
    //check_kernel();
    std::string errStr = "";
    for(size_t i = 0 ; i < CMainCtrl::Get().m_errList.size(); i++)
    {
        //errStr += CMainCtrl::Get().m_errList.at(i);
        CTajiDbMgr::Get().AddSysLog(CMainCtrl::Get().m_errList.at(i));
    }
    if(errStr.length() > 0)
    {
        _finished = false;

        //MessageBox(m_hWnd,errStr.c_str(), "error",MB_OK);
    }

}
void   CInitForm::OnKeyUp(int scancode, DWORD shiftstate)
{
    if(scancode == SCANCODE_ENTER)
    {
        //OnTimer(100);
    }
}
void   CInitForm::OnShow()
{

    device_check();

    //if(_finished)
    //    SetTimer(m_hWnd,100,10);
    //else
    //{
        SetTimer(m_hWnd,100,1);
    //}
        //启动主控制模块
}

void   CInitForm::OnTimer(int ID)
{
    TIniFile cfg("ctx2000.conf");

    if(cfg.ReadBool("display","isCacheBmp",false))
    {
        CIconGroup grp;
        grp.AddIcons(preLoadBmp,sizeof(preLoadBmp)/sizeof(const char*));
    }

    KillTimer(m_hWnd,100);

    Close();

}
