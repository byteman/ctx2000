
/**
 * \file hw_engine.c
 * \author Qingzhou Long <qzlong@minigui.org>
 * \date 2005/12/22
 *
 * This file includes input method interfaces of mGi. 
 */

/*
 * $Id: hw_engine.c 104 2006-01-23 01:20:44Z qzlong $
 *          mGi (MiniGUI Inputmethod) is a component of MiniGUI.
 *
 *          MiniGUI is a compact cross-platform Graphics User Interface 
 *         (GUI) support system for real-time embedded systems.
 *                  
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <minigui/common.h>

#include "hw_engine.h"

/* null engine */
static int null_recognize(WORD* pTrace,char* pResult,
                    int *nCands,int nPoints, WORD wRange)
{
    memcpy (pResult, "a\0b\0c\0d\0e\0f\0g\0h\0", 16);
    if (nCands)
        *nCands = 8;

    return 0;
}

static int null_init(void)
{
    return 0;
}

static int null_exit(void)
{
    return 0;
}

MGI_HW_ENGINE hw_null_engine = {
    null_init,
    null_recognize,
    null_exit
};

