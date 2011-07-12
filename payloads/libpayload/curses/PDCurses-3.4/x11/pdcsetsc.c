/* Public Domain Curses */

#include "pdcx11.h"

RCSID("$Id: pdcsetsc.c,v 1.33 2008/07/14 04:24:52 wmcbrine Exp $")

#include <string.h>

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
    int ret_vis = SP->visibility;

    PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", visibility));

    if (visibility != -1)
        SP->visibility = visibility;

    PDC_display_cursor(SP->cursrow, SP->curscol, SP->cursrow,
                       SP->curscol, visibility);

    return ret_vis;
}

void PDC_set_title(const char *title)
{
    int len;

    PDC_LOG(("PDC_set_title() - called:<%s>\n", title));

    len = strlen(title) + 1;        /* write nul character */

    XCursesInstruct(CURSES_TITLE);

    if (XC_write_display_socket_int(len) >= 0)
        if (XC_write_socket(xc_display_sock, title, len) >= 0)
            return;

    XCursesExitCursesProcess(1, "exiting from PDC_set_title");
}

int PDC_set_blink(bool blinkon)
{
    if (pdc_color_started)
        COLORS = 16;

    return blinkon ? ERR : OK;
}
