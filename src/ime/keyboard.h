/**
 * \file keyboard.h
 * \author Qingzhou Long <qzlong@minigui.org>
 * \date 2005/12/22
 *
 * This file includes input method interfaces of mGi. 
 */

/*
 * $Id: keyboard.h 100 2006-01-17 02:21:25Z qzlong $
 *          mGi (MiniGUI Inputmethod) is a component of MiniGUI.
 *
 *          MiniGUI is a compact cross-platform Graphics User Interface 
 *         (GUI) support system for real-time embedded systems.
 *                  
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEFT    0
#define TOP     36
#define MSG_MYIME_CLOSE	    (0x0000ffff + 200 + 1)
#define MSG_MYIME_OPEN	    (0x0000ffff + 200 + 2)
#define MSG_KEYBOARD_CLOSE   (0x0000ffff + 200 + 3)
#define MSG_KEYBOARD_OPEN    (0x0000ffff + 200 + 4)

#define OPS_NAME_LENGTH     20
#define NR_INPUTMETHOD      10

    /**
     * \addtogroup Enums soft-keyboard key type enums
     * @{
     */

/** 
 * The key of soft-keyboard's type.
 */
typedef enum {
    /**
     * Scancode type key.
     * You can click this kind of key to send a scancode.
     */
    SFT_SCANCODE,
    /**
     * String type key.
     * You can click this kind of key to send a string.
     */
    SFT_STRING,
} SOFTKEYTYPE;

    /** @} end of Enums */

    /**
     *      * \addtogroup fns Functions
     *           * @{
     *                */

    /**
     *      * \defgroup sk_fns Soft-keyboard input method functions
     *           * @{
     *                */

/** Each Soft-keyboard Key Information */
typedef struct _MGI_SOFTKEY_INFO {
    /** Key type */
    SOFTKEYTYPE type;
    /** The string that this key represents,
     *  available if type is SFT_STRING
     */
    char* str;
    /** The scancode that this key represents,
     *  available if type is SFT_SCANCODE
     */
    int scancode;

    /** The key rect in the keyboard bitmap */
    RECT rect;

    /** Key status */
    int status;
} MGI_SOFTKEY_INFO;

/**
 * \var typedef void (*MGI_CB_Translate_Init) (void) 
 * \brief Type of the translation method init callback procedure.
 *
 * This is the function type of translation method init callback procedure.
 * If you set a new translation method for the soft-keyboard, you might init
 * the resources in this function
 *
 */
typedef void (*MGI_CB_Translate_Init) (void);

/**
 * \var typedef void (*MGI_CB_Translate_Destroy) (void) 
 * \brief Type of the translation method destroy callback procedure.
 *
 * This is the function type of translation method destroy callback procedure.
 * If you set a new translation method for the soft-keyboard, you might destroy
 * the resources finally in this function.
 *
 */
typedef void (*MGI_CB_Translate_Destroy) (void);

/**
 * \var typedef void (*MGI_CB_Translate_Draw) (HWND hwnd, HDC hdc, void* context) 
 * \brief Type of the translation method draw callback procedure.
 *
 * This is the function type of translation method draw callback procedure.
 * If you set a new translation method for the soft-keyboard, you should draw
 * the relative contents in this function.
 *
 */
typedef void (*MGI_CB_Translate_Draw) (HWND hwnd, HDC hdc, void* context);

/**
 * \var typedef int (*MGI_CB_Translate_Alpha)(const char* origin_str, char* ret_value, LPARAM lParam, BOOL* write_directly)
 * \brief Type of the translation method alpha-translate callback procedure.
 *
 * This is the function type of translation method alpha-translate callback procedure.
 * If you set a new translation method for the soft-keyboard, you should translate 
 * alpha in this function.
 *
 */
typedef int (*MGI_CB_Translate_Alpha) (const char* origin_str, char* ret_value,
                                       LPARAM lParam, BOOL* write_directly);

/**
 * \var typedef int (*MGI_CB_Translate_Number) (const char* origin_str, char* ret_value, BOOL* write_directly)
 * \brief Type of the translation method number-translate callback procedure.
 *
 * This is the function type of translation method number-translate callback procedure.
 * If you set a new translation method for the soft-keyboard, you should translate 
 * number in this function.
 *
 */
typedef int (*MGI_CB_Translate_Number) (const char* origin_str, char* ret_value, 
                                        BOOL* write_directly);

/**
 * \var typedef int (*MGI_CB_Translate_Punc) (const char origin_str, char* ret_value) 
 * \brief Type of the translation method punctuation-translate callback procedure.
 *
 * This is the function type of translation method punctuation-translate callback procedure.
 * If you set a new translation method for the soft-keyboard, you should translate 
 * punctuation in this function.
 *
 */
typedef int (*MGI_CB_Translate_Punc) (const char origin_str, char* ret_value);

/**
 * \var typedef int (*MGI_CB_Associate) (const char* origin_str, char* ret_value) 
 * \brief Type of the translation method associate callback procedure.
 *
 * This is the function type of translation method associate callback procedure.
 * If you set a new translation method for the soft-keyboard, you should fetch
 * associate words in this function.
 *
 */
typedef int (*MGI_CB_Associate) (const char* origin_str, char* ret_value);

/** Translation Operation Information */
typedef struct _MGI_TRANSLATE_OPS {
    /** Name of this trans-ops */
    char ops_name[OPS_NAME_LENGTH];
    /** whether this trans-ops is for single byte charset */
    BOOL is_ascii;

    /** Initialization operation */
    MGI_CB_Translate_Init cb_init;
    /** Destroy operation */
    MGI_CB_Translate_Destroy cb_destroy;
    /** Draw operation */
    MGI_CB_Translate_Draw cb_draw;
    /** Alpha-translation operation */
    MGI_CB_Translate_Alpha cb_trans_alpha;
    /** Number-translation operation */
    MGI_CB_Translate_Number cb_trans_num;
    /** Punctuation-translation operation */
    MGI_CB_Translate_Punc cb_trans_punc;
    /** Associate operation */
    MGI_CB_Associate cb_associate;
} MGI_TRANSLATE_OPS;

/** Soft-keyboard Information */
typedef struct _MGI_SOFTKEYBOARD_INFO {
    /** The bitmap used by the soft-keyboard */
    BITMAP skb_bmp;
    /** All keys of the soft-keyboard */
    MGI_SOFTKEY_INFO* keys;
    /** Numbers of key */
    int nr_keys;
    /** Layout of keyboard */
    char* kbd_layout;

    /** Current translation ops index */
    int cur_trans_ops;
    /** Translation opses */
    MGI_TRANSLATE_OPS* trans_ops[NR_INPUTMETHOD];
} MGI_SOFTKEYBOARD_INFO;

typedef struct _KEYEVENT
{
    int     scancode;
    DWORD   status;
} KEYEVENT;

extern MGI_TRANSLATE_OPS en_trans_ops;
extern MGI_TRANSLATE_OPS quanpin_trans_ops;

/**
 * \fn HWND mgiCreateSoftKeyboardIME (MGI_SOFTKEYBOARD_INFO* sk_info, PLOGFONT ime_font) 
 * \brief Create the soft-keyboard input method window.
 *
 * You can create the soft-keyboard input method window by this function.
 *
 * \param sk_info  the soft-keyboard information.
 * \param ime_font the font that the IMEwindow uses.
 *
 * \return Return the handle of this ime window.
 *
 */
extern HWND mgiCreateSoftKeyboardIME (MGI_SOFTKEYBOARD_INFO* sk_info, PLOGFONT ime_font);

/**
 * \fn BOOL mgiAddSKBTranslateOps (HWND ime_hwnd, MGI_TRANSLATE_OPS* trans_ops) 
 * \brief Add a suite of translation operation to the soft-keyboard IME window.
 *
 * You can add a suite of translation operation to the soft-keyboard IME window
 * by this function.
 *
 * \param ime_hwnd  the soft-keyboard window handle.
 * \param trans_ops the suite of translation operation gonna add.
 *
 * \return Return TRUE if added successfully, FALSE otherwise.
 *
 */
extern BOOL mgiAddSKBTranslateOps (HWND ime_hwnd, MGI_TRANSLATE_OPS* trans_ops);

    /** @} end of sk_fns */

    /** @} end of fns */

extern int     init_soft_keyboard(void);
extern void    refresh_soft_keyboard_bk(HWND hwnd, HDC hdc);
extern void    refresh_soft_keyboard_key(HWND hwnd, HDC hdc);
extern int     soft_keyboard_mouse_down(HWND hWnd, int x, int y, KEYEVENT * ke);
extern int     soft_keyboard_down(HWND hWnd, int, KEYEVENT * ke);
extern int     soft_keyboard_up(HWND hWnd, KEYEVENT * ke);
extern void    move_ime_window(HWND hWnd, HWND target);
extern int     active_soft_key(void *context, HWND hWnd, int message, WPARAM wParam,
                    LPARAM lParam);
extern int cursor_direct(HWND hWnd, int scancode);
extern void update_input_window(HWND hWnd);
extern void destroy_soft_keyboard(void);

#ifdef __cplusplus
}
#endif

#endif /* __KEYBOARD_H__ */
