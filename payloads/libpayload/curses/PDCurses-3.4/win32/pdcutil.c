/* Public Domain Curses */

#include "pdcwin.h"

RCSID("$Id: pdcutil.c,v 1.14 2008/07/14 04:24:52 wmcbrine Exp $")

void PDC_beep(void)
{
    PDC_LOG(("PDC_beep() - called\n"));

/*  MessageBeep(MB_OK); */
    MessageBeep(0XFFFFFFFF);
}

void PDC_napms(int ms)
{
    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

    Sleep(ms);
}

const char *PDC_sysname(void)
{
    return "Win32";
}
