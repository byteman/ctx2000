
/**
 * \file mgime.h
 * \author Qingzhou Long <qzlong@minigui.org>
 * \date 2005/12/22
 *
 * This file includes input method interfaces of mGi. 
 */

/*
 * $Id: mgime.h 100 2006-01-17 02:21:25Z qzlong $
 *          mGi (MiniGUI Inputmethod) is a component of MiniGUI.
 *
 *          MiniGUI is a compact cross-platform Graphics User Interface 
 *         (GUI) support system for real-time embedded systems.
 *                  
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef __HZIME_H__
#define __HZIME_H__

#ifdef __cplusplus
extern  "C"
{
#endif                          /* __cplusplus */

    /**
     * \addtogroup defs Defines
     * @{
     */
/** BackGround color of IME window*/
#define INPUT_BGCOLOR           8          /* LightBlack */
/** ForeGround color of IME window*/
#define INPUT_FGCOLOR           15         /* LightWhite */

#define INPUT_AREAY          ((18 * 24)+10)

#define MAX_INPUT_LENGTH        15

#define MAGIC_NUMBER            "CCEGB"
#define CIN_ENAME_LENGTH        24
#define CIN_CNAME_LENGTH        16
#define MAX_PHRASE_LENGTH       20
#define SELECT_KEY_LENGTH       16
#define END_KEY_LENGTH          16
#define MAX_SEL_LENGTH 			58
/** Max number of selections */
#define MAXDUPSEL               7

#define InputAreaX              16

/** Max length of IME's name */
#define MAX_IME_NAME    20

    /** @} end of defs */

    /**
     *  \defgroup container_win_ncs Notification codes of IME container window 
     *  @{
     */

/**
 * \def MGIN_SKIME
 * \brief The MGIN_SKIME notification message is sent when the user clicks
 *        the soft-keyboard icon on the IME container window.
 */
#define MGIN_SKIME      300

/**
 * \def MGIN_HWIME 
 * \brief The MGIN_HWIME notification message is sent when the user clicks
 *        the handwrite icon on the IME container window.
 */
#define MGIN_HWIME      301

    /** @} end of container_win_ncs */


    /**
     * \addtogroup Enums IME Windows adding status enums
     * @{
     */

/** 
 * \enum _IME_WINDOW_STATUS
 * \brief The enum specifies the status of adding a new IME window 
 */
enum _IME_WINDOW_STATUS {
    /**
     * IME window added successfully.
     */
    IME_WINDOW_SUCCESS,
    /**
     * the IME window that gonna add is invalid
     * or the container hasn't been created yet.
     */
    IME_WINDOW_INVALID,
    /**
     * the IME window has already added before. 
     */
    IME_WINDOW_EXISTED,
    /**
     * the IME window container is full. 
     */
    IME_WINDOW_FULL,
};

    /** @} end of Enums */

/* key of toggle input method */

typedef struct
{
    unsigned long key1;     /* sizeof(ITEM=12) */
    unsigned long key2;
    unsigned short ch;
    unsigned short frequency;
} _ITEM;

typedef struct
{
    char    magic_number[sizeof(MAGIC_NUMBER)];     /* magic number */
    char    ename[CIN_ENAME_LENGTH];        /* ascii name */
    char    cname[CIN_CNAME_LENGTH];        /* prompt */
    char    selkey[SELECT_KEY_LENGTH];      /* select keys */

    char    last_full;      /* last full code need a more SPACE? */
    int     TotalKey;       /* number of keys needed */
    int     MaxPress;       /* Max len of keystroke */
    int     MaxDupSel;      /* how many keys used to select */
    int     TotalChar;      /* Defined characters */

    unsigned char KeyMap[128];      /* Map 128 chars to 64(6 bit) key index */
    unsigned char KeyName[64];      /* Map 64 key to 128 chars */
    unsigned short KeyIndex[64];    /* 64 key first index of TotalChar */

    int     PhraseNum;      /* Total Phrase Number */
    FILE   *PhraseFile;     /* *.tab.phr Phrase File */
    FILE   *AssocFile;      /* *.tab.lx LianXiang File */
    _ITEM   *item;           /* item */
} mgime_input_table;

typedef struct
{
    short   l, t, r, b;
} _hit_addr;

void    __mg_ime_outchar(unsigned char ch, LPARAM lParam);

    /**
     *      * \addtogroup fns Functions
     *           * @{
     *                */

    /**
     *      * \defgroup ime_container_fns IME Window container functions
     *           * @{
     *                */

/** IME Window Information */
typedef struct _IME_WIN {
    /** Handle of IME Window */
    HWND ime_hwnd;
    /** Name of IME Window */
    char ime_name[MAX_IME_NAME];
} IME_WIN;

/** IME Window Container Information */
typedef struct _IME_CONTAINER {
    /** The IME container window */
    HWND cont_hwnd;
    /** The max number of IME Windows that the container contains */
    int max_ime_windows;
    /** The current IME window ID */
    int cur_wins;
    /** IME Window Information */
    IME_WIN* ime_wins;
} IME_CONTAINER;

/**
 * \fn GHANDLE mgiCreateIMEContainer (int max_ime_wins, BOOL with_cont_win) 
 * \brief Create the container of input method windows.
 *
 * You can create a container of input method windows by this function, and
 * you can specify the max input method window numbers of this container.
 *
 * \param max_ime_wins  the max ime window numbers of this container.
 * \param with_cont_win if show the container window.
 *
 * \return Return the handle of this ime windows container.
 *
 * \sa mgiDestroyIMEContainer 
 */
GHANDLE mgiCreateIMEContainer (int max_ime_wins, BOOL with_cont_win);

/**
 * \fn int mgiAddIMEWindow (GHANDLE ime, HWND ime_hwnd, const char* ime_name) 
 * \brief Add a ime window to the container.
 *
 * You can add a input method window to the certain container by this function.
 *
 * \param ime       the handle of ime windows container.
 * \param ime_hwnd  the handle of this ime window.
 * \param ime_name  the name of this ime window.
 *
 * \return Return the handle of this ime windows container.
 *
 * \sa mgiRemoveIMEWindow 
 */
int mgiAddIMEWindow (GHANDLE ime, HWND ime_hwnd, const char* ime_name);

/**
 * \fn BOOL mgiRemoveIMEWindow (GHANDLE ime, const char* ime_name) 
 * \brief Remove a ime window from the container.
 *
 * You can remove a input method window from the certain container by this function.
 *
 * \param ime       the handle of ime windows container.
 * \param ime_name  the name of this ime window.
 *
 * \return Return TRUE if the ime window has been removed successfully,
 *         FALSE otherwise.
 *
 * \sa mgiAddIMEWindow 
 */
BOOL mgiRemoveIMEWindow (GHANDLE ime, const char* ime_name);

/**
 * \fn BOOL mgiSetActiveIMEWindow (GHANDLE ime, const char* ime_name)
 * \brief Set a ime window actived.
 *
 * You can set a input method window actived by this function.
 *
 * \param ime       the handle of ime windows container.
 * \param ime_name  the name of this ime window.
 *
 * \return Return TRUE if set actived successfully, FALSE otherwise.
 *
 */
BOOL mgiSetActiveIMEWindow (GHANDLE ime, const char* ime_name);

/**
 * \fn NOTIFPROC mgiSetNotificationCallback (GHANDLE ime, NOTIFPROC notif_proc) 
 * \brief Set the ime windows container window's notification callback.
 *
 * You can set the ime windows container window's notification callback
 * if you created the container window.
 *
 * \param ime           the handle of ime windows container.
 * \param notif_proc    the new notification callback of the container window
 *
 * \return Return old notification callback of the container window
 *
 * \sa mgiCreateIMEContainer 
 */
NOTIFPROC mgiSetNotificationCallback (GHANDLE ime, NOTIFPROC notif_proc);

/**
 * \fn BOOL mgiDestroyIMEContainer (GHANDLE ime)
 * \brief Destroy the ime windows container.
 *
 * You can destroy the ime windows container by this function.
 *
 * \param ime       the handle of ime windows container.
 *
 * \return Return TRUE if destroy successfully, FALSE otherwis.
 *
 * \sa mgiCreateIMEContainer 
 */
BOOL mgiDestroyIMEContainer (GHANDLE ime);


    /** @} end of ime_container_fns */

    /** @} end of fns */


#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif /* __HZIME_H__ */
