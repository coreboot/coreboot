/* Public Domain Curses */

#include "pdcos2.h"

RCSID("$Id: pdcutil.c,v 1.14 2008/07/14 04:24:51 wmcbrine Exp $")

#if defined(OS2) && !defined(__EMX__)
APIRET APIENTRY DosSleep(ULONG ulTime);
#endif

void PDC_beep(void)
{
    PDC_LOG(("PDC_beep() - called\n"));

#ifdef EMXVIDEO
    putchar('\007');
#else
    DosBeep(1380, 100);
#endif
}

void PDC_napms(int ms)
{
    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));

#ifdef __EMX__
    _sleep2(ms);
#else
    DosSleep(ms);
#endif
}

const char *PDC_sysname(void)
{
    return "OS/2";
}
