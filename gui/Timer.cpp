#include "TimerItem.h"
#include "Timer.h"
#include "BmpRes.h"
#include "SoftKeyboard.h"
extern SoftKeyboard* skt;
static COMM_CTRL_DESC commctrls[] =
{
    EDIT_HOUR,
    EDIT_MINUTE,
    EDIT_SECOND,
};

static const char* mmenu_bmps[] =
{
    SYSCFG_TIMER_BACKGROUND,
    PCOMM_OK_BTN,
    PCOMM_CANCEL_BTN,
    PCOMM_CLOSE_BTN
};

static SKIN_BUTTON_DESC SkinBtnsDesc[] =
{
    SKIN_BUTTON_OK,
    SKIN_BUTTON_CANCEL,
    SKIN_BUTTON_EXIT
};

static const char *icon_path[] =
{
    SYSCFG_SITE_ICON,
    SYSCFG_SITE_LABEL,
    PCOMM_LINE_ICON,
    SYSCFG_TIMESET_TEXT
};

static POS IconsPos[] =
{
    {
        SYSCFG_X,
        SYSCFG_Y
    },
    {
        SYSCFG_TEXT_X,
        SYSCFG_TEXT_Y
    },
    {
        LINE_X,
        LINE_Y
    },
    {
        SYSCFG_TIMESET_X,
        SYSCFG_TIMESET_Y
    }
};
#ifdef ENGLISH_VER
static const char *msg_path[] =
{
    "../resource/english/syscfgform/timer/hour.png",
    "../resource/english/syscfgform/timer/minute.png",
    "../resource/english/syscfgform/timer/second.png"
};
#else
static const char *msg_path[] =
{
    "../resource/chinese/syscfgform/timer/hour.png",
    "../resource/chinese/syscfgform/timer/minute.png",
    "../resource/chinese/syscfgform/timer/second.png"
};
#endif

static skin_bmp_t Msgs[] =
{
    {
        HOUR_X,
        HOUR_Y,
        0,
        0,
    },
    {
        MINUTE_X,
        MINUTE_Y,
        0,
        0,
    },
    {
        SECOND_X,
        SECOND_Y,
        0,
        0,
    }
};

CTimer::CTimer()
{
    if (!LoadRes(&mmenu_bmps[0], ARRAY_SIZE(mmenu_bmps, char *)))
    {
        printf("Can't loadres\n");
        exit(0);
    }

    m_hCalendar = HWND_INVALID;
    m_timeChanged = false;
    m_dateChanged = false;
//	m_tailindex = 0;
    m_rect.left = RECT_LEFT;
    m_rect.top = RECT_TOP;
    m_rect.right = RECT_RIGHT;
    m_rect.bottom = RECT_BOTTOM;

    for (int i = 0; i < TIMER_EDIT_NUM; i++)
        _edit[i] = new CEdit(&commctrls[i],this);

    for (int j = 0; j < TIMER_BTN_NUM; j++)
        _skinBtns[j] = new CSkinButton(&SkinBtnsDesc[j],this);

    _lables.AddIcons(icon_path,ARRAY_SIZE(icon_path,char*));

    for (int k = 0; k < SKIN_MSG_NUM; k++)
        LoadBitmapFromFile(HDC_SCREEN, &(Msgs[k].m_bmp), msg_path[k]);

    InitSkinHeader("CTimer");


}

CTimer::~CTimer()
{
    for (int k = 0; k < SKIN_MSG_NUM; k++)
        UnloadBitmap(&(Msgs[k].m_bmp));

    //  [8/13/2010 zgy]
    for (int j = 0; j < TIMER_BTN_NUM; j++)
    {
        delete _skinBtns[j];
    }

    for (int i = 0; i < TIMER_EDIT_NUM; i++)
    {
        delete _edit[i];
    }

    KillTimer(m_hWnd,100);

}

void    CTimer::OnCreate()
{
    if(HWND_INVALID==m_hCalendar)
    {

        m_hCalendar = CreateWindowEx(CALENDAR_CLASS,CALENDAR_CAPTION,CALENDAR_STYLE, \
                                     CALENDAR_EXSTYLE,CALENDAR_ID,CALENDAR_X,CALENDAR_Y, CALENDAR_W, CALENDAR_H,m_hWnd,CALENDAR_ADDDATA );
        SetTimer(m_hWnd,100,50);
    }
}

void    CTimer::OnTimer(int ID)
{
    time_t t;
    struct tm *p_time;
    time(&t);
    p_time = localtime(&t);

//	m_year = p_time->tm_year+1900;
//	m_mon  = p_time->tm_mon+1;
//	m_day  = p_time->tm_mday;

    m_hour = p_time->tm_hour;
    m_min  = p_time->tm_min;
    m_sec  = p_time->tm_sec;

    if(!m_timeChanged)
    {
        char buf[10]= {0,};
        sprintf(buf,"%02d", m_hour);
        _edit[0]->SetText(buf);
        sprintf(buf,"%02d", m_min);
        _edit[1]->SetText(buf);
        sprintf(buf,"%02d", m_sec);
        _edit[2]->SetText(buf);
    }
    InvalidateRect(m_hWnd, &m_rect, true);
//	UpdateWindow(m_hWnd, true);
    HDC hdc = BeginPaint(m_hWnd);
    int temphour;
    temphour = m_hour;
    if(temphour>11)
        temphour -= 12;
    PivotBitmap(hdc,&(Msgs[0].m_bmp),Msgs[0].x,Msgs[0].y,5,60,30*64*temphour + 32*m_min);
    PivotBitmap(hdc,&(Msgs[1].m_bmp),Msgs[1].x,Msgs[1].y,5,60,6*64*m_min);
    PivotBitmap(hdc,&(Msgs[2].m_bmp),Msgs[2].x,Msgs[2].y,5,60,6*64*m_sec);
    EndPaint(m_hWnd, hdc);
}

void CTimer::OnCommCtrlNotify(HWND hwnd, int id, int nc)
{
    if((EN_CHANGE == nc)&&((_edit[0]->GetId()==id)||(_edit[1]->GetId()==id)||(_edit[2]->GetId()==id)))
    {
        m_timeChanged = true;
        m_hour = atoi(_edit[0]->GetText().c_str());
        m_min = atoi(_edit[1]->GetText().c_str());
        m_sec = atoi(_edit[2]->GetText().c_str());
        //AchillesDBG("%d,%d,%d\n", m_hour, m_min, m_sec);
        char buf[10]= {0,};
        if(m_hour>23)
        {
            m_hour = 23;
            sprintf(buf,"%02d", m_hour);
            _edit[0]->SetText(buf);
        }
        if(m_min>59)
        {
            m_min = 59;
            sprintf(buf,"%02d", m_min);
            _edit[1]->SetText(buf);
        }
        if(m_sec>59)
        {
            m_sec = 59;
            sprintf(buf,"%02d", m_sec);
            _edit[2]->SetText(buf);
        }
    }
    if(nc==EN_SETFOCUS)
    {
        fprintf(stderr,"setfocus\n");
        if(skt)
        {
            //fprintf(stderr,"t9show\n");
            skt->T9_Show(true);
        }
    }
}

void    CTimer::OnPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);

    for (int i = 0; i< SKIN_BMP_NUM; i++)
        _lables.Show(hdc,IconsPos[i].x,IconsPos[i].y,i+1);
    EndPaint(hWnd, hdc);
}

void    CTimer::OnClose()
{
    m_hCalendar = HWND_INVALID;
}

void
CTimer::OnShow()
{
    for(int i = 0; i < TIMER_EDIT_NUM; i++)
        _edit[i]->SetLimitLength(2);
    for(int i = 0; i < TIMER_EDIT_NUM; i++)
        _edit[i]->EnableDigtal(true);
}

void    CTimer::OnButtonClick(skin_item_t* item)
{
    if(item->id == _skinBtns[0]->GetId())
    {
        if(m_dateChanged)
        {
            char bufdate[60]= {0,};
            sprintf(bufdate, "date -s \"%02d%02d%02d%02d%04d.%02d\"", m_mon, m_day, m_hour, m_min, m_year,m_sec);

            system(bufdate);
            system("hwclock --systohc");
        }
        if(m_timeChanged)
        {
            char buftime[40]= {0,};
            m_hour = atoi(_edit[0]->GetText().c_str());
            m_min = atoi(_edit[1]->GetText().c_str());
            m_sec = atoi(_edit[2]->GetText().c_str());
            sprintf(buftime, "date -s %d:%d:%d", m_hour, m_min, m_sec);
            system(buftime);
            system("hwclock --systohc");
        }
        Close();
    }
    else
    {
        Close();

    }
}

void CTimer::OnCommand(HWND hWnd, int message, WPARAM wParam,LPARAM lParam)
{
    int id = LOWORD(wParam);
    if(CALENDAR_ID == id)
    {
        m_day = SendMessage(m_hCalendar, MCM_GETCURDAY, 0, 0);
        m_mon = SendMessage(m_hCalendar, MCM_GETCURMONTH, 0, 0);
        m_year = SendMessage(m_hCalendar, MCM_GETCURYEAR, 0, 0);
        char buf[32];
        sprintf(buf,"%4d-%2d-%2d",m_year,m_mon,m_day);
        m_dateChanged = true;
    }
}



