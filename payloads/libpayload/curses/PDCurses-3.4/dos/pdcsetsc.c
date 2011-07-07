/* Public Domain Curses */

#include "pdcdos.h"

RCSID("$Id: pdcsetsc.c,v 1.39 2008/07/13 16:08:17 wmcbrine Exp $")

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
    PDCREGS regs;
    int ret_vis, start, end;

    PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", visibility));

    ret_vis = SP->visibility;
    SP->visibility = visibility;

    switch (visibility)
    {
        case 0:  /* invisible */
            start = 32;
            end = 0;  /* was 32 */
            break;
        case 2:  /* highly visible */
            start = 0;   /* full-height block */
            end = 7;
            break;
        default:  /* normal visibility */
            start = (SP->orig_cursor >> 8) & 0xff;
            end = SP->orig_cursor & 0xff;
    }

    /* if scrnmode is not set, some BIOSes hang */

    regs.h.ah = 0x01;
    regs.h.al = (unsigned char)pdc_scrnmode; 
    regs.h.ch = (unsigned char)start;
    regs.h.cl = (unsigned char)end;
    PDCINT(0x10, regs);

    return ret_vis;
}

void PDC_set_title(const char *title)
{
    PDC_LOG(("PDC_set_title() - called: <%s>\n", title));
}

int PDC_set_blink(bool blinkon)
{
    PDCREGS regs;

    switch (pdc_adapter)
    {
    case _EGACOLOR:
    case _EGAMONO:
    case _VGACOLOR:
    case _VGAMONO:
        regs.W.ax = 0x1003;
        regs.W.bx = blinkon;

        PDCINT(0x10, regs);

        if (pdc_color_started)
            COLORS = blinkon ? 8 : 16;

        break;
    default:
        COLORS = 8;
    }

    return (COLORS - (blinkon * 8) != 8) ? OK : ERR;
}
