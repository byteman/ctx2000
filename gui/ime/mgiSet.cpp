#include "mgiSet.h"
#include "ime_pinyin.h"
#include "mgpti.h"
#include <ctype.h>

#define MSG_PHONE_METHOD_SWITCH MSG_USER+13
#define MSG_PHONE_METHOD_STATUS MSG_USER+14

Method_Switch_Notify CallbackFun;

static int  mgpti_case;
static HWND hwnd_phone_ime = HWND_INVALID;

static int  Mgi_Pti_Match_Keystrokes(void *method_data, const char *strokes, char *buffer, int buffer_len, int index);
static int  Mgi_Pti_Translate_Word(void *method_data, const char *strokes, char *buffer, int buffer_len, int index);
static int  Mgi_Pti_Status_Changed(void *method_data, DWORD status);
static int  Mgi_Pti_Active(void *method_data,BOOL is_active);

static int pinyin_get_symbol(void *method_data, char *buff, int buff_len);

static MGI_PHONE_KEY_MAP phone_map[MAX_PHONE_KEY_NUM] = 
{
    {SCANCODE_CURSORBLOCKLEFT, ""},
    {SCANCODE_CURSORBLOCKRIGHT, ""},
    {SCANCODE_CURSORBLOCKUP, ""},
    {SCANCODE_CURSORBLOCKDOWN, ""},
    {SCANCODE_BACKSPACE, ""},
    {SCANCODE_ENTER, ""},
    {SCANCODE_F8, ""},
    {SCANCODE_F9, ""},
    {SCANCODE_0, " 0"},
    {SCANCODE_1, ",.;:!?\"'-()@/_+&%*=<>$[]{}\\~^`1"},
    {SCANCODE_2, "abc2"},
    {SCANCODE_3, "def3"},
    {SCANCODE_4, "ghi4"},
    {SCANCODE_5, "jkl5"},
    {SCANCODE_6, "mno6"},
    {SCANCODE_7, "pqrs7"},
    {SCANCODE_8, "tuv8"},
    {SCANCODE_9, "wxyz9"},
};

static MGI_PHONE_IME_METHOD mgpti_method = {
    INPUT_EN,
    "mgpti",
    NULL,
    Mgi_Pti_Match_Keystrokes,
    Mgi_Pti_Translate_Word,
    NULL,
    Mgi_Pti_Status_Changed,
    Mgi_Pti_Active,
    NULL,
    NULL,
    NULL
};

static MGI_PHONE_IME_METHOD py_method = 
{
    INPUT_PY,
    "pinyin",
    NULL,
    pinyin_match_keystokes,
    pinyin_translate_word,
    pinyin_predict_pord,
    NULL,
    NULL,
    pinyin_get_symbol,
    NULL,
    NULL
};

const MGI_PHONE_IME_TEXTCOLOR phone_textcolor = {
    PIXEL_lightgray,
    PIXEL_black,
    PIXEL_lightgray,
    PIXEL_lightgray,
    PIXEL_black,
    PIXEL_lightgray,
};

int Mgi_Pti_Match_Keystrokes(void *method_data, const char *strokes, char *buffer, int buffer_len, int index)
{
    //return ptiMatchIcwKeystokes(strokes, buffer, index, PTI_CASE_abc);
    return ptiMatchKeystokes(strokes, buffer, index, mgpti_case);
}

int Mgi_Pti_Translate_Word(void *method_data, const char *strokes, char *buffer, int buffer_len, int index)
{
    char tmp[30+1]={0};
            
    if (strokes[strlen(strokes)-1] != '*') {
        if (strlen(strokes) < buffer_len - 2)
            sprintf(buffer, "%s ", strokes);
        else {
            strncpy(buffer, strokes, buffer_len);
            buffer[buffer_len - 1] = ' ';
            buffer[buffer_len] = '\0';
        }
        return 0;
    }

    strncpy(tmp, strokes, sizeof(tmp)-1);
    if (mgpti_case == PTI_CASE_Abc) {
        tmp[0] = tolower(tmp[0]);
    }
    else if (mgpti_case == PTI_CASE_ABC) {
        int i;
        for(i=0;i<strlen(strokes);i++)
            tmp[i]=tolower(tmp[i]);
    }
    return ptiMatchKeystokes(tmp, buffer, index, mgpti_case);
}

int Mgi_Pti_Status_Changed(void *method_data, DWORD status)
{
    HWND htargetwnd;
    if (mgpti_case < PTI_CASE_abc)
        mgpti_case++;
    else
        mgpti_case = PTI_CASE_ABC;

#if defined(_MGRM_PROCESSES) && !defined(_STAND_ALONE)
    Send2ActiveWindow (mgTopmostLayer, MSG_PHONE_METHOD_STATUS, (DWORD)mgpti_case, 0);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
    htargetwnd = SendMessage(hwnd_phone_ime, MSG_IME_GETTARGET, 0, 0);
    PostMessage(htargetwnd, MSG_PHONE_METHOD_STATUS, (DWORD)mgpti_case, 0 );
#endif
    
    return 0;
}

int Mgi_Pti_Active(void *method_data,BOOL is_active)
{
    return 0;
}

int pinyin_get_symbol(void *method_data, char *buff, int buff_len)
{
    //char punctuate[] = "， 。 、 ： ； ！ · ？ ＃ ￥ ％ ℃ ～ × （ ） 〔 〕 《 》 〈 〉 【 】 〖 〗 『";
    char punctuate[] = "， 。 、 ： ； ！ · ？ ‘ ’ “ ” ＃ ￥ ％ ℃ ～ × （ ） 〔 〕 …… ── 《 》 〈 〉 【 】 〖 『 』 「 」 ";

    if (buff == NULL || buff_len <= 0)
        return -1;
    if (strlen(punctuate) < buff_len)
        strcpy(buff, punctuate);
    else {
        strncpy(buff, punctuate, buff_len);
        buff[buff_len+1] = '\0';
    }
    return strlen(buff);
}

int Mgphone_Method_Switch_Notify(void *method_data, int new_method_id)
{
#if 0
#if defined(_MGRM_PROCESSES) && !defined(_STAND_ALONE)
    Send2ActiveWindow (mgTopmostLayer, MSG_PHONE_METHOD_SWITCH, (DWORD)new_method_id, 0);
    if (new_method_id == pti_method.method_id)
        Send2ActiveWindow (mgTopmostLayer, MSG_PHONE_METHOD_STATUS, (DWORD)pti_case, 0);
    else
        Send2ActiveWindow (mgTopmostLayer, MSG_PHONE_METHOD_STATUS, 0, 0);

#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
    HWND htargetwnd = SendMessage(phone_ime_hwnd, MSG_IME_GETTARGET, 0, 0);
    PostMessage(htargetwnd, MSG_PHONE_METHOD_SWITCH, (DWORD)new_method_id, 0 );
    if (new_method_id == mpti_method.method_id)
        PostMessage(htargetwnd, MSG_PHONE_METHOD_STATUS, (DWORD)mpti_case, 0);
    else
        PostMessage(htargetwnd, MSG_PHONE_METHOD_STATUS, 0, 0);
#endif
    return 0;
#else
    HWND htargetwnd = SendMessage(hwnd_phone_ime, MSG_IME_GETTARGET, 0, 0);
    PostMessage(htargetwnd, MSG_PHONE_METHOD_SWITCH, (DWORD)new_method_id, 0 );
    if (new_method_id == mgpti_method.method_id) {
        PostMessage(htargetwnd, MSG_PHONE_METHOD_STATUS, (DWORD)mgpti_case, 0);
    } else {
        PostMessage(htargetwnd, MSG_PHONE_METHOD_STATUS, 0, 0);
    }
    
    if (CallbackFun) {
        //DPRINTF(JINFO, "Mgphone_Method_Switch_Notify  %d\n",new_method_id);
        CallbackFun(new_method_id);
    }
#endif
}

mgiSet::mgiSet(int left ,int top ,int right,int bottom)
{
    RECT rect;
    
    CallbackFun = NULL;
    ime_handle = mgiCreateIMEContainer (2, FALSE);
    SetRect(&rect, left, top, right, bottom);

//T9输入法
    mgpti_case=PTI_CASE_abc;
    Create_Phone_IME(&rect);
//全拼输入法
#if 0
    memset (&sk_info, 0, sizeof (MGI_SOFTKEYBOARD_INFO));
    sk_ime_hwnd = mgiCreateSoftKeyboardIME(&sk_info, NULL);
    mgiAddSKBTranslateOps (sk_ime_hwnd, &quanpin_trans_ops);
    
    mgiAddIMEWindow (ime_handle, sk_ime_hwnd, "softkeyboard");
#endif
    mgiAddIMEWindow (ime_handle, hwnd_phone_ime,"mgphone");
    mgiSetActiveIMEWindow (ime_handle, "mgphone");
}

mgiSet::~mgiSet()
{
    mgiDestroyIMEContainer (ime_handle);
}

void
mgiSet::switchinput(int input)
{
    mgiPhoneKeyPadSetCurrMethod(input);
}

void
mgiSet::move(int x,int y)
{
    RECT crect;
    if (hwnd_phone_ime == HWND_INVALID)
        return;
    GetWindowRect(hwnd_phone_ime, &crect);
    MoveWindow(hwnd_phone_ime,
               x, y,
               crect.right-crect.left, crect.bottom-crect.top,
               TRUE);
}
HWND
mgiSet::Create_Phone_IME(RECT *rect)
{
    HWND hphonewnd;
    BITMAP bmp;

    //LoadBitmapFromFile(HDC_SCREEN, &bmp, "bkg.bmp");
    //plogfont = CreateLogFontIndirect
    ime_pinyin_init(); 
    hphonewnd = mgiCreatePhoneKeyPadIME(rect,
                                        NULL,
                                        NULL,
                                        NULL,
                                        (const MGI_PHONE_KEY_MAP (*)[MAX_PHONE_KEY_NUM])phone_map,
                                        Mgphone_Method_Switch_Notify);
    
    hwnd_phone_ime = hphonewnd;

    //mgiPhoneKeyPadAddMethod(hphonewnd, &mgpti_method);
    //mgiPhoneKeyPadAddMethod(hphonewnd, &py_method);
    #if 1
    if ( mgiPhoneKeyPadAddMethod(hphonewnd, &py_method) ) 
        mgiPhoneKeyPadSetCurrMethod(mgpti_method.method_id);
    else
        fprintf(stderr, "mgiPhoneKeyPadAddMethod failed\n");
    #endif

    return hphonewnd;
}

void 
mgiSet::Register_Callback_Fun(Method_Switch_Notify func) {
    if (func) {
        CallbackFun = func;
    }
}

    
