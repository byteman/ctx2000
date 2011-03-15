
/**
 * \file hwinput.h
 * \author Qingzhou Long <qzlong@minigui.org>
 * \date 2005/12/22
 *
 * This file includes input method interfaces of mGi. 
 */

/*
 * $Id: hwinput.h 103 2006-01-20 07:08:01Z qzlong $
 *          mGi (MiniGUI Inputmethod) is a component of MiniGUI.
 *
 *          MiniGUI is a compact cross-platform Graphics User Interface 
 *         (GUI) support system for real-time embedded systems.
 *                  
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

/* ========================================================================= 
 * hand write input module definitions and strutures.
   ========================================================================= */

#ifndef __FREEDRAW_H__
#define __FREEDRAW_H__

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define DEF_HW_WIDTH    280
#define DEF_HW_HEIGHT   150

#define CTRL_HWI    "hwi"
/* full screen hand write support */
#define SUPPORTS_FULLSCR_HW	1

#define REC_RANGE     ((HW_NUMERIC) | (HW_LCALPHA) | (HW_UCALPHA) | (HW_PUNCTUATION) | (HW_SYMBOLS) | (HW_CHINESE_COMMON) | (HW_CHINESE_RARE) |(HW_CHINESE_VARIANT))
#define FROM_HWINPUT 0x10000000
#define IS_CHINESE   0x01000000

#define NWORD        10
#define MAXSIZE      300
#define NALLOC       MAXSIZE 
#define THRESHHOLD_MIN    9
#define THRESHHOLD_MAX    169 

#define THRESHHOLD_FULLSCREEN	100

#define SPACE            2

#if SUPPORTS_FULLSCR_HW
  #define HW_MODE_NORMAL	0
  #define HW_MODE_FULLSCR	1

  #define HW_FULL_ON_WRITE	0
  #define HW_FULL_WRITE_OVER	1

//message to set or get the current handwrite mode
#define MSG_HW_SETMODE		(MSG_USER+20)
#define MSG_HW_GETMODE		(MSG_USER+21)
//message to show or hide the transparent window
#define MSG_HW_SHOW		(MSG_USER+22)
#define MSG_HW_SETWRITE		(MSG_USER+23)
#define MSG_HW_GETSTATUS	(MSG_USER+24)

//handwrite window height in the fullscreen mode
#define FULLSCR_HW_WND_H	16

#ifdef PDA_SIZE_160x160
#define FULL_NWORD        6
#else
#define FULL_NWORD        10 
#endif

#endif

#include <minigui/common.h>
#include "hw_engine.h"

#define NUMBER_RECOG_RANGE           0x0001
#define UPPERCASE_RECOG_RANGE        0x0002
#define LOWERCASE_RECOG_RANGE        0x0004
#define INTERPUNCTION_RECOG_RANGE    0x0008
#define ADAPTATION_RECOG_RANGE       0x1000

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \defgroup hw_fns Handwrite input method functions
     * @{
     */

/** Handwrite Information */
typedef struct _MGI_HANDWRITE_INFO {
    /** Handwrite window's left top x coordinate */
    int x;
    /** Handwrite window's left top y coordinate */
    int y;
    /** Handwrite window's width */
    int w;
    /** Handwrite window's height */
    int h;

    /** Whether the Handwrite window has two drawing area */
    BOOL two_wins;
    /** The handwrite recognization engine */
    MGI_HW_ENGINE* engine;
} MGI_HANDWRITE_INFO;

/**
 * \fn HWND mgiCreateHandWriteIME (MGI_HANDWRITE_INFO* hw_info, PLOGFONT ime_font) 
 * \brief Create the handwrite input method window.
 *
 * You can create the handwrite input method window by this function.
 *
 * \param hw_info  the handwrite information.
 * \param ime_font the font that the IMEwindow uses.
 *
 * \return Return the handle of this ime window.
 *
 */
HWND mgiCreateHandWriteIME (MGI_HANDWRITE_INFO* hw_info, PLOGFONT ime_font);

/**
 * \fn BOOL mgiSetHandWriteEngine (HWND hw_ime_hwnd, MGI_HW_ENGINE* engine) 
 * \brief Set the handwrite input method recognization engine.
 *
 * You can set the handwrite input method recognization engine by this function.
 *
 * \param hw_ime_hwnd  the handle of handwrite window.
 * \param engine       the recognization engine gonna set.
 *
 * \return Return TRUE if set successfully, FALSE otherwise.
 *
 */
BOOL mgiSetHandWriteEngine (HWND hw_ime_hwnd, MGI_HW_ENGINE* engine);

    /** @} end of hw_fns */

    /** @} end of fns */

/* struct defines */

typedef struct tagCHARINFO {
    BYTE    code[2];
    RECT    rc;
}CHARINFO;

typedef struct tagHWINFO {
    WORD *     m_data;
    int        m_size; 
    int        m_len; 

    BOOL       capture;
    int        old_x;
    int        old_y;

    BYTE       result[NWORD*2 + 2];
    //BITMAP     keybitmap;
    BYTE       fitchar[NWORD][3];
    HWND       phwnd;

    RECT       *rcDraw;
    //RECT       rcClear;
    CHARINFO   charinfo[NWORD];
    
    RECT       rcchar; 
/* added by snig for new hw */
    RECT       rcl;
    RECT       rcr;
    char       active_rc;
    int        nword;    /* words to recognize */
    HWND       hDrawWnd; /* handle of the current draw window */
    HWND       hHWI;
/****************************/

    BOOL two_wins;
    MGI_HW_ENGINE* engine;
} HWINFO;

typedef HWINFO* PHWINFO;

BOOL RegisterHWIControl (void);
void HWIControlCleanup (void);
int HWIMECtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

void on_command (HWND, int);

extern MGI_HW_ENGINE hw_null_engine;

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__FREEDRAW_H__
