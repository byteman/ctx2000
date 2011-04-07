/**
 * \file hw_engine.h
 * \author Qingzhou Long <qzlong@minigui.org>
 * \date 2005/12/22
 *
 * This file includes input method interfaces of mGi. 
 */

/*
 * $Id: hw_engine.h 100 2006-01-17 02:21:25Z qzlong $
 *          mGi (MiniGUI Inputmethod) is a component of MiniGUI.
 *
 *          MiniGUI is a compact cross-platform Graphics User Interface 
 *         (GUI) support system for real-time embedded systems.
 *                  
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef _HW_ENGINE_H
#define _HW_ENGINE_H

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

    /**
     * \addtogroup hw_fns Handwrite input method functions
     * @{
     */

/** Handwrite Engine Information */
typedef struct _MGI_HW_ENGINE
{
    /** Initialization operation of the engine */
    int (*init)(void);
    /** Recognize operation of the engine */
    int (*recognize)(WORD* pTrace,char* pResult,int *nCands, int nPoints, WORD wRange);
    /** Exit operation of the engine */
    BOOL (*exit)(void);
} MGI_HW_ENGINE;

    /** @} end of sk_fns */
#endif /* _HW_ENGINE_H */
