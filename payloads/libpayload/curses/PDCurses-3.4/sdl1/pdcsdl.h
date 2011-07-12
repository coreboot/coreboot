/* Public Domain Curses */

/* $Id: pdcsdl.h,v 1.17 2008/07/14 04:24:52 wmcbrine Exp $ */

#include <curspriv.h>

#include <SDL/SDL.h>

PDCEX SDL_Surface *pdc_screen, *pdc_font, *pdc_icon, *pdc_back;
PDCEX int pdc_sheight, pdc_swidth, pdc_yoffset, pdc_xoffset;

extern SDL_Surface *pdc_tileback;    /* used to regenerate the background
                                        of "transparent" cells */
extern SDL_Color pdc_color[16];      /* colors for font palette */
extern Uint32 pdc_mapped[16];        /* colors for FillRect(), as
                                        used in _highlight() */
extern int pdc_fheight, pdc_fwidth;  /* font height and width */
extern int pdc_flastc;               /* font palette's last color
                                        (treated as the foreground) */
extern bool pdc_own_screen;          /* if pdc_screen was not set
                                        before initscr(), PDCurses is
                                        responsible for (owns) it */
extern Uint32 pdc_lastupdate;        /* time of last update, in ticks */

void PDC_update_rects(void);
void PDC_retile(void);
