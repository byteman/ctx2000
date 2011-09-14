#include "SoftKeyboardForm.h"
extern SoftKeyboard *g_SKB;

#define CELL_W      44
#define CELL_H      43
#define KEY_X       11
#define KEY_Y       20
#define D_CELL_W    52
#define D_CELL_H    51
#define KEYNUM      17

static int keymap[KEYNUM] = {
    SCANCODE_1, SCANCODE_2, SCANCODE_3, SCANCODE_0, SCANCODE_CURSORBLOCKUP, SCANCODE_F9,
    SCANCODE_4, SCANCODE_5, SCANCODE_6, SCANCODE_CURSORBLOCKLEFT, SCANCODE_CURSORBLOCKDOWN, SCANCODE_CURSORBLOCKRIGHT,
    SCANCODE_7, SCANCODE_8, SCANCODE_9, SCANCODE_BACKSPACE,  SCANCODE_ENTER
};
static RECT keyret[KEYNUM];

static const char *SoftKeyboardForm_path[] = {
    "../resource/chinese/ime/keyboard.png",
    "../resource/chinese/ime/ziti.png"
};
static int oldx, oldy;
static si_bmplabel_t InputSta={"0","12340"};
static BOOL fCaptured = FALSE;
enum __SKINID_SoftKeyboardForm{
    SKINID_SoftKeyboardForm_START = 0,
    SKINID_SoftKeyboardForm_ABC,
    SKINID_SoftKeyboardForm_END,
};
enum __SKINBMPINDEX_SoftKeyboardForm{
    SKINBMP_SoftKeyboardForm_ABC= 1,
};

 pthread_t SoftKeyboardForm::winthread;
 _SoftKeyboardForm SoftKeyboardForm::WinInfo;

int  SoftKeyboardForm::inputmet = 0;

static SKIN_BUTTON_DESC skinctrls[] = {
     {1,273,20}
};
#include <vector>

typedef std::vector<char>  subimevct;
class imeitem{
public:
    imeitem(std::string name)
    {
        ime_name=name;
        sub_imes.clear ();
        //ime_type=IME_CLOSE;
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
    int subime_bmp_idx;  //该输入法子状态图标在图片中的序号
    size_t subime_idx;  //该输入法的子状态的序号0,1,2..
    subimevct sub_imes; //子状态列表
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
        ime_bmp_idx   = 0;
        subime_bmp_idx= 0;
        subime_idx    = 0;
        //ime_type=IME_T9;
        sub_imes.push_back (0);
        sub_imes.push_back (3);
        sub_imes.push_back (4);
        sub_imes.push_back (1);
    }
};
class ime_mgr{
public:
    ime_mgr()
    {
        imes.push_back (new t9_item("mgphone"));
        //imes.push_back (new quanpin_item("quanpin"));
        //imes.push_back (new close_item(""));
        cur_ime_idx        = 0;
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
        //std::cerr << "changeIme " << cur_ime_type << std::endl;
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
void SoftKeyboardForm::CreateIMEWindow()
{
    if (!LoadRes(&SoftKeyboardForm_path[0], ARRAY_SIZE(SoftKeyboardForm_path, char *)))
    {
            printf("Can't loadres\n");
            exit(0);
    }
    m_mask1.LoadFile("../resource/chinese/ime/bts.png");
    m_mask2.LoadFile("../resource/chinese/ime/btmaskl.png");
    m_status = new CSkinLable(&skinctrls[0],this,"skinlable",(void*)&InputSta);
    InitSkinHeader("SoftKeyboardForm");

    //fprintf(stderr,"x=%d,y=%d,w=%d,h=%d\n",WinInfo.lx,WinInfo.ly,WinInfo.rx,WinInfo.by);
    m_move_able = true;
    this->CreateForm(HWND_DESKTOP,WinInfo.lx,WinInfo.ly,WinInfo.rx,WinInfo.by,TRUE,WS_EX_TOPMOST|WS_EX_TOOLWINDOW);

}
void *
SoftKeyboardForm::SoftKeyboardForm_Start(void *data)
{
    if(data)
    {
        SoftKeyboardForm* form = (SoftKeyboardForm*)data;
        form->CreateIMEWindow();
    }
    return NULL;
}

void SoftKeyboardForm::Init()
{
    int i,x,y;
    //m_skin_head.hwnd = HWND_INVALID;

    x = KEY_X;
    y = KEY_Y;
    for (i = 0; i < 6; i++) {
        SetRect(&keyret[i], x, y, x+CELL_W, y+CELL_H);
        x += D_CELL_W;
    }
    x = KEY_X;
    y = KEY_Y + D_CELL_H;
    for (i = 6; i < 12; i++) {
        SetRect(&keyret[i], x, y, x+CELL_W, y+CELL_H);
        x += D_CELL_W;
    }
    x = KEY_X;
    y = KEY_Y + D_CELL_H + D_CELL_H;
    for (i = 12; i < 15; i++) {
        SetRect(&keyret[i], x, y, x+CELL_W, y+CELL_H);
        x += D_CELL_W;
    }
    SetRect(&keyret[15], 168, 127, 238, 200);
    SetRect(&keyret[16], 246, 127, 316, 200);
}

HWND SoftKeyboardForm::CreateFormEx(HWND hWnd, int lx, int ly, int rx, int by,DWORD dwExStyle,
						   BOOL bModal)
{
    pthread_attr_t new_attr;

    sem_init (&WinInfo.wait, 0, 0);
    WinInfo.hWnd    = hWnd;
    WinInfo.lx      = lx;
    WinInfo.ly      = ly;
    WinInfo.rx      = lx+329;
    WinInfo.by      = ly+170;
    WinInfo.dwExStyle = dwExStyle;
    WinInfo.bModal  = bModal;

    pthread_attr_init (&new_attr);
    pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
    pthread_create (&winthread, &new_attr, SoftKeyboardForm_Start,(void*)this);
    pthread_attr_destroy (&new_attr);

    sem_wait (&WinInfo.wait);
    sem_destroy (&WinInfo.wait);

    return WinInfo.hWnd;
}


void 
SoftKeyboardForm::Close()
{

}

void SoftKeyboardForm::OnLButtonUp(int x, int y)
{
    if (fCaptured) {
       ReleaseCapture();
       fCaptured = FALSE;
   } else {
       UpdateWindow(m_hWnd,TRUE);
   }

}
void SoftKeyboardForm::OnMouseMove(int x, int y)
{
    if (fCaptured)
    {
        RECT rcWindow;
        GetWindowRect(m_hWnd, &rcWindow);
        OffsetRect(&rcWindow, x - oldx, y - oldy);
        if(m_move_able)
        {
            MoveWindow(m_hWnd, rcWindow.left, rcWindow.top, RECTW(rcWindow),
                       RECTH(rcWindow), TRUE);
        }


        oldx = x;
        oldy = y;
    }
}

void SoftKeyboardForm::OnLButtonDown(int x, int y)
{

    oldx = x;
    oldy = y;
    //fprintf(stderr,"x=%d,y=%d\n",x,y);
    int ret = Key_Check(oldx,oldy);
    if (ret != -1) {
      PostMessage(HWND_DESKTOP,MSG_KEYDOWN,keymap[ret],0);
      HDC hdc = GetClientDC(m_hWnd);
      fprintf(stderr,"x1=%d,y1=%d ret = %d\n",keyret[ret].left,keyret[ret].top,ret);
      if( (ret == 15) || (ret == 16))
          m_mask2.Show(hdc,keyret[ret].left,keyret[ret].top);
      else
          m_mask1.Show(hdc,keyret[ret].left,keyret[ret].top);
      if( ret == 5 )
      {
         inputmet = imemgr.changeStatus();
         char cbuf[2]={0,};
         sprintf(cbuf,"%d",inputmet);
         m_status->Display(cbuf);
      }
      ReleaseDC(hdc);
    } else {

          SetCapture(m_hWnd);
          ClientToScreen(m_hWnd, &oldx, &oldy);
          fCaptured = TRUE;

    }
    if(ret == 16){
        ShowWindow(m_hWnd,SW_HIDE);
    }
}
void SoftKeyboardForm::OnShow()
{
     RECT rc;
     GetWindowRect(m_hWnd, &rc);
     WinInfo.hWnd = m_hWnd;
     sem_post(&WinInfo.wait);
}

int
SoftKeyboardForm::Key_Check(int x , int y)
{
    //fprintf(stderr,"x=%d,y=%d,w=%d,h=%d\n",keyret[0].right,keyret[0].left,keyret[0].top,keyret[0].bottom);
    for(int i = 0; i < KEYNUM; i++) {

        if((x > keyret[i].left) && (x < keyret[i].right) && (y > keyret[i].top) && (y < keyret[i].bottom))
            return i;
    }
    return -1;
}
    
void  SoftKeyboardForm::SetInputMethod(int method)
{
    if(m_status == NULL)
        return;
    switch (method) {

    case T9_NUM:
        m_status->Display("0");
        break;
    case T9_ABC:
        m_status->Display("3");
        break;
    case T9_abc:
        m_status->Display("4");
        break;
    case T9_EN:
        m_status->Display("2");
        break;
    case T9_PY:
        m_status->Display("1");
        break;
    default:
        break;
    }
}
