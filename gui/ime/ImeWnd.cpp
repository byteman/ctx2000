#include "ImeWnd.h"
#include <vector>
#include <assert.h>
#include "mgime.h"
#define CONT_WIN_WIDTH      90
#define CONT_WIN_HEIGHT     30
#define IME_CLOSE   0
#define IME_QUANPIN 1
#define IME_T9      2

#define IMELOGO_LEFT        3
#define IMELOGO_TOP         5

#define IMERECT_LEFT        30
#define IMERECT_TOP         1
#define IME_HEIGHT          25
#define IME_WIDTH           24

#define SUBIMERECT_LEFT     IMERECT_LEFT+IME_WIDTH+5
#define SUBIMERECT_TOP       1


static int old_x,old_y;
static CIMEWnd* imeInstance=NULL;
static pthread_t cont_thread;
extern HWND create_test_phone_ime_wnd();
static HWND gbime_handle,t9_ime_hwnd,imecon_handle;
static bool fCaptured=false;

typedef std::vector<char>  subimevct;
class imeitem{
public:
    imeitem(std::string name)
    {
        ime_name=name;
        sub_imes.clear ();
        ime_type=IME_CLOSE;
        ime_bmp_idx=1;
        subime_idx=0;
    }
    int changeStatus()
    {
        if(sub_imes.size () == 0 )
            return -1;
        subime_idx++;
        if(subime_idx >= sub_imes.size ())
            subime_idx = 0;
        subime_bmp_idx = sub_imes.at (subime_idx);
        return subime_bmp_idx;
    }
    int ime_bmp_idx;
    int subime_bmp_idx;
    size_t subime_idx;
    subimevct sub_imes;
    std::string ime_name;
    int ime_type;
};
typedef std::vector<imeitem*>  imevct;
class t9_item:public imeitem
{
public:
    t9_item(std::string name)
        :imeitem(name)
    {
        ime_bmp_idx = 8;
        subime_bmp_idx=6;
        subime_idx=3;
        ime_type=IME_T9;
        sub_imes.push_back (2);
        sub_imes.push_back (3);
        sub_imes.push_back (4);
        sub_imes.push_back (6);
    }
};
class quanpin_item:public imeitem
{
public:
    quanpin_item(std::string name)
        :imeitem(name)
    {
        ime_bmp_idx = 7;
        subime_bmp_idx=6;
        subime_idx=1;
        ime_type=IME_QUANPIN;
        sub_imes.push_back (5);
        sub_imes.push_back (6);
    }
};
class close_item:public imeitem{
public:
    close_item(std::string name)
        :imeitem(name)
    {
        ime_bmp_idx = 1;
        subime_bmp_idx=5;
        subime_idx=0;
        ime_type=IME_CLOSE;
        sub_imes.push_back (5);
    }
};
class ime_mgr{
public:
    ime_mgr()
    {
        imes.push_back (new t9_item("mgphone"));
        imes.push_back (new quanpin_item("quanpin"));
        imes.push_back (new close_item(""));
        cur_ime_idx        = 2;
        cur_ime_type       = imes.at(cur_ime_idx)->ime_type;
        cur_ime_bmp_idx    = imes.at(cur_ime_idx)->ime_bmp_idx;
        cur_subime_bmp_idx = imes.at(cur_ime_idx)->subime_bmp_idx;

    }
    ~ime_mgr()
    {
        for(size_t i = 0; i < imes.size (); i++)
        {
            if(imes.at (i))
            {
                delete imes.at(i);
            }
        }
    }
    int changeIme()
    {
        cur_ime_idx++;
        if( cur_ime_idx >= imes.size () )
        {
            cur_ime_idx = 0;
        }
        cur_ime_bmp_idx    = imes.at(cur_ime_idx)->ime_bmp_idx;
        cur_subime_bmp_idx = imes.at(cur_ime_idx)->subime_bmp_idx;
        cur_ime_type       = imes.at (cur_ime_idx)->ime_type;
        std::cerr << "changeIme " << cur_ime_type << std::endl;
        return cur_ime_type;
    }
    int changeStatus()
    {
        if((cur_ime_idx >=0 ) && (cur_ime_idx < imes.size ()))
        {

            int ret = imes.at (cur_ime_idx)->changeStatus();
            cur_subime_bmp_idx = imes.at(cur_ime_idx)->subime_bmp_idx;
            return ret;
        }
        assert("cur_ime invalid\n");
        return 0;
    }

    imevct imes;
    int  cur_ime_idx;

    int cur_ime_bmp_idx;
    int cur_subime_bmp_idx;
    int cur_ime_type;
};
static ime_mgr imemgr;
static void draw_border (HWND hwnd, HDC hdc)
{
    SetPenColor (hdc, COLOR_darkgray);

    LineEx (hdc, 1, 0, 1, CONT_WIN_HEIGHT);
    LineEx (hdc, CONT_WIN_WIDTH-1, 0, CONT_WIN_WIDTH-1, CONT_WIN_HEIGHT);
}
static void changeInputMothed(HWND hwnd)
{

    PostMessage(hwnd,MSG_USER_IME_STATUS,0,0);
}
int keyboardHook(void* context, HWND dst_wnd,
                int msg, WPARAM wparam, LPARAM lparam)
{
    //fprintf(stderr,"keycode=%d lparam = %d\n",wparam,lparam);
    CIMEWnd* form = (CIMEWnd*)context;

    if( (wparam == SCANCODE_F8) && (msg == MSG_KEYDOWN))
    {
        changeInputMothed(form->m_hWnd);
    }else if(wparam==SCANCODE_LEFTCONTROL)
    {
        /*quanping change msg is key up*/
        if( (imemgr.cur_ime_type == IME_QUANPIN) && (msg == MSG_KEYDOWN))
        {
            return HOOK_GOON;
        }
        /* T9 change msg is key down*/
        if( (imemgr.cur_ime_type == IME_T9) && (msg == MSG_KEYUP))
        {
            return HOOK_GOON;
        }
        imemgr.changeStatus ();
        InvalidateRect (form->m_hWnd,&form->refRect,TRUE);
    }
    return HOOK_GOON;
}

int CIMEWnd::InitIME()
{

    gbime_handle = GBIMEWindow(HWND_DESKTOP);

    MoveWindow(gbime_handle, IME_X, IME_Y, IME_W, IME_H, FALSE);

    imecon_handle = mgiCreateIMEContainer (2, FALSE);
    if(imecon_handle == 0)
    {
        fprintf(stderr,"mgiCreateIMEContainer failed\n");
        return -1;
    }
    t9_ime_hwnd = create_test_phone_ime_wnd();
    if(t9_ime_hwnd == HWND_INVALID)
    {
        fprintf(stderr,"create_phone_ime_wnd failed\n");
        return -2;
    }
    if(mgiAddIMEWindow (imecon_handle, t9_ime_hwnd, "mgphone") != IME_WINDOW_SUCCESS)
    {
          fprintf(stderr,"Add mgphone IME failed\n");
          return -3;
    }
    if(mgiAddIMEWindow (imecon_handle, gbime_handle,   "quanpin") != IME_WINDOW_SUCCESS)
    {
            fprintf(stderr,"Add quanpin IME failed\n");
            return -4;
    }
    if(mgiSetActiveIMEWindow (imecon_handle, ""))
    {
        fprintf(stderr,"Active quanpin IME failed\n");
        return -5;
    }
    //ShowWindow(t9_ime_hwnd,SW_HIDE);
    //mgiSetActiveIMEWindow(imecon_handle,"");
    //gImeStatus.setime (m_hWnd,&refRect,IME_CLOSE);
    //SendMessage(m_hWnd,MSG_USER_IME_STATUS,IME_CLOSE,0);

    return 0;
}
int CIMEWnd::Key_Check(int x, int y)
{
    if (PtInRect (&imeRect,x,y))
        return 1;
    else if(PtInRect (&subimeRect,x,y)){
        return 2;
    }
    return 0;
}

int CIMEWnd::MsgProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {
        case MSG_PAINT:
            hdc = BeginPaint (hwnd);
            draw_border (hwnd, hdc);
            logo.Show(hdc, IMELOGO_LEFT,IMELOGO_TOP,1);
            icon.Show (hdc,imeRect.left,   imeRect.top,   imemgr.cur_ime_bmp_idx);
            icon.Show (hdc,subimeRect.left,subimeRect.top,imemgr.cur_subime_bmp_idx);
            EndPaint (hwnd, hdc);
            break;
        case MSG_LBUTTONDOWN:
        {
               old_x = LOWORD(lParam);
               old_y = HIWORD(lParam);
               int ret = Key_Check(old_x,old_y);

               if(ret == 1)
               {
                    changeInputMothed(hwnd);
               }else if(ret == 2)
               {
                  if(imemgr.cur_ime_type == IME_QUANPIN)
                  {
                      SendMessage(HWND_DESKTOP,MSG_KEYDOWN,SCANCODE_LEFTCONTROL,0);
                      SendMessage(HWND_DESKTOP,MSG_KEYUP,SCANCODE_LEFTCONTROL,0);
                  }else if(imemgr.cur_ime_type == IME_T9){
                      SendMessage(HWND_DESKTOP,MSG_KEYDOWN,SCANCODE_LEFTCONTROL,0);
                      SendMessage(HWND_DESKTOP,MSG_KEYUP,SCANCODE_LEFTCONTROL,0);
                  }else{
                     //关闭状态不用发消息
                  }
              }else{
                SetCapture (hwnd);
                ClientToScreen (hwnd,&old_x,&old_y);
                fCaptured=true;
              }
               break;
           }
        case MSG_LBUTTONUP:
        {
                if(fCaptured){
                    ReleaseCapture ();
                    fCaptured=false;
                }
        }
        break;

    case MSG_MOUSEMOVE:
    {
        if (fCaptured)
        {
            RECT rcWindow;
            GetWindowRect(hwnd, &rcWindow);
            int x = LOSWORD(lParam);
            int y = HISWORD(lParam);
/*
            if( rcWindow.left < 0)
                rcWindow.left = 0;
            if(rcWindow.right > 800)
                rcWindow.right = 800;
            if( rcWindow.top < 0)
                rcWindow.top = 0;
            if(rcWindow.bottom > 480)
                rcWindow.bottom = 480;
                */
            OffsetRect(&rcWindow, x - old_x, y - old_y);
            //if( (rcWindow.left > 0) && (rcWindow.right < 800) &&  (rcWindow.top > 0) && (rcWindow.bottom < 480))
            {
                MoveWindow(hwnd, rcWindow.left, rcWindow.top, RECTW(rcWindow),
                           RECTH(rcWindow), TRUE);
                //RECT rc;
                //GetWindowRect(hWnd, &rc);
                //DPRINTF(JINFO, "rc %d %d\n",rc.left,rc.top);
                //g_SKB->move(rc.left, rc.top - 50);
                old_x = x;
                old_y = y;
            }

        }
        break;
    }
        case MSG_USER_IME_STATUS:
            {
                int type = imemgr.changeIme ();
                //fprintf(stderr,"get type %d \n",type);
                if(type == IME_CLOSE){
                    //std::cerr << "ime close\n";
                    mgiSetActiveIMEWindow(imecon_handle,"");
                }else if(type == IME_QUANPIN){
                    mgiSetActiveIMEWindow(imecon_handle,"quanpin");
                }else if(type == IME_T9){
                    //fprintf(stderr,"change to mgphone\n");
                    if(!mgiSetActiveIMEWindow(imecon_handle,"mgphone"))
                    {
                        fprintf(stderr,"change ime to mgphone faild\n");
                    }else
                    {
                        SendNotifyMessage (t9_ime_hwnd, MSG_IME_OPEN, 0, 0);
                        ShowWindow (t9_ime_hwnd,SW_SHOW);
                    }

                }
                InvalidateRect (hwnd,&refRect,TRUE);
            }
            break;
    };
    return DefaultMainWinProc (hwnd, message, wParam, lParam);
}

int mgi_container_win_proc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{

    if(imeInstance)
        return  imeInstance->MsgProc (hwnd,message,wParam,lParam);
    return -1;
}
void* start_cont_win (void* data)
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    HWND cont_hwnd;
    CONTAINER_INFO* cont_info = (CONTAINER_INFO*)data;

    CreateInfo.dwStyle = WS_ABSSCRPOS | WS_VISIBLE;
    CreateInfo.dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
    CreateInfo.spCaption = "";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;

    CreateInfo.MainWindowProc = mgi_container_win_proc;
    CreateInfo.lx = g_rcScr.right-CONT_WIN_WIDTH-20;
    CreateInfo.rx = g_rcScr.right-20;
    CreateInfo.ty = 0;
    CreateInfo.by = CONT_WIN_HEIGHT;
    CreateInfo.iBkColor = GetWindowElementColor(BKC_CONTROL_DEF);
    CreateInfo.hHosting = HWND_DESKTOP;

    cont_hwnd = cont_info->hwnd = CreateMainWindow (&CreateInfo);
    sem_post (&cont_info->wait);

    if (cont_hwnd == HWND_INVALID)
        return NULL;

    while (GetMessage (&Msg, cont_hwnd) ) {
        TranslateMessage (&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (cont_hwnd);
    return NULL;

}


HWND create_container_win ()
{
    CONTAINER_INFO cont_info;
    pthread_attr_t new_attr;

    sem_init (&cont_info.wait, 0, 0);

    pthread_attr_init (&new_attr);
    pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
    pthread_create (&cont_thread, &new_attr, start_cont_win, &cont_info);
    pthread_attr_destroy (&new_attr);

    sem_wait (&cont_info.wait);
    sem_destroy (&cont_info.wait);

    return cont_info.hwnd;
}
CIMEWnd::CIMEWnd()
{

}
int CIMEWnd::StartIME ()
{
    icon.LoadFile ("comm/ime.gif",8);
    logo.LoadFile ("comm/imelogo.bmp",1);
    imeInstance = this;
    SetRect (&imeRect,IMERECT_LEFT,IMERECT_TOP,IMERECT_LEFT+IME_WIDTH,IMERECT_TOP+IME_HEIGHT);
    SetRect (&subimeRect,SUBIMERECT_LEFT,SUBIMERECT_TOP,SUBIMERECT_LEFT+IME_WIDTH,SUBIMERECT_TOP+IME_HEIGHT);
    SetRect(&refRect,IMERECT_LEFT,IMERECT_TOP,IMERECT_LEFT+2*IME_WIDTH+5,IMERECT_TOP+IME_HEIGHT);
    m_hWnd=create_container_win ();
    if(m_hWnd != HWND_INVALID)
    {
        int ret = InitIME();
        if(ret != 0)
            return ret;
        RegisterKeyMsgHook(this,keyboardHook);
        return 0;
    }
    return -1;
}
bool CIMEWnd::MoveWindowPos(int x, int y,BOOL fPaint)
{
    return MoveWindow(m_hWnd,x,y,CONT_WIN_WIDTH,CONT_WIN_HEIGHT,fPaint);
}
