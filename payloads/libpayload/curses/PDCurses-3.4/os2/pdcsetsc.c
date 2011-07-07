/* Public Domain Curses */

#include "pdcos2.h"

RCSID("$Id: pdcsetsc.c,v 1.44 2008/07/14 04:24:51 wmcbrine Exp $")

/*man-start**************************************************************

  Name:                                                         pdcsetsc

  Synopsis:
        int PDC_set_blink(bool blinkon);
        void PDC_set_title(const char *title);

  Description:
        PDC_set_blink() toggles whether the A_BLINK attribute sets an
        actual blink mode (TRUE), or sets the background color to high
        intensity (FALSE). The default is platform-dependent (FALSE in
        most cases). It returns OK if it could set the state to match 
        the given parameter, ERR otherwise. Current platforms also 
        adjust the value of COLORS according to this function -- 16 for 
        FALSE, and 8 for TRUE.

        PDC_set_title() sets the title of the window in which the curses
        program is running. This function may not do anything on some
        platforms. (Currently it only works in Win32 and X11.)

  Portability                                X/Open    BSD    SYS V
        PDC_set_blink                           -       -       -
        PDC_set_title                           -       -       -

**man-end****************************************************************/

int PDC_curs_set(int visibility)
{
#ifndef EMXVIDEO
    VIOCURSORINFO pvioCursorInfo;
#endif
    int ret_vis, hidden = 0, start = 0, end = 0;

    PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", visibility));

    ret_vis = SP->visibility;
    SP->visibility = visibility;

    switch(visibility)
    {
    case 0:     /* invisible */
#ifdef EMXVIDEO
        start = end = 0;
#else
        start = pdc_font / 4;
        end = pdc_font;
        hidden = -1;
#endif
        break;

    case 2:     /* highly visible */
        start = 2;      /* almost full-height block */
        end = pdc_font - 1;
        break;

    default:    /* normal visibility */
        start = (SP->orig_cursor >> 8) & 0xff;
        end = SP->orig_cursor & 0xff;
    }

#ifdef EMXVIDEO
    if (!visibility)
        v_hidecursor();
    else
        v_ctype(start, end);
#else
    pvioCursorInfo.yStart = (USHORT)start;
    pvioCursorInfo.cEnd = (USHORT)end;
    pvioCursorInfo.cx = (USHORT)1;
    pvioCursorInfo.attr = hidden;
    VioSetCurType((PVIOCURSORINFO)&pvioCursorInfo, 0);
#endif
    return ret_vis;
}

void PDC_set_title(const char *title)
{
    PDC_LOG(("PDC_set_title() - called:<%s>\n", title));
}

int PDC_set_blink(bool blinkon)
{
#ifndef EMXVIDEO
    USHORT statebuf[3], result;

    statebuf[0] = 6;    /* length */
    statebuf[1] = 2;    /* blink/intensity */
    statebuf[2] = !blinkon;

    result = VioSetState(&statebuf, 0);
    VioGetState(&statebuf, 0);  /* needed? */

    if (pdc_color_started)
        COLORS = statebuf[2] ? 16 : 8;

    return (result == 0) ? OK : ERR;
#else
    if (pdc_color_started)
        COLORS = 16;

    return blinkon ? ERR : OK;
#endif
}
