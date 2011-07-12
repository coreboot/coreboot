/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2011 secunet Security Networks AG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "lppdc.h"

#include <stdlib.h>

#ifdef CHTYPE_LONG
# define PDC_OFFSET 32
#else
# define PDC_OFFSET  8
#endif

/* COLOR_PAIR to attribute encoding table. */

unsigned char *pdc_atrtab = (unsigned char *)NULL;

short curstoreal[16], realtocurs[16] =
{
    COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN, COLOR_RED,
    COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE, COLOR_BLACK + 8,
    COLOR_BLUE + 8, COLOR_GREEN + 8, COLOR_CYAN + 8, COLOR_RED + 8,
    COLOR_MAGENTA + 8, COLOR_YELLOW + 8, COLOR_WHITE + 8
};

/* close the physical screen -- may restore the screen to its state 
   before PDC_scr_open(); miscellaneous cleanup */

void PDC_scr_close(void)
{
    PDC_LOG(("PDC_scr_close() - called\n"));

    reset_shell_mode();

    if (SP->visibility != 1)
        curs_set(1);

    /* Position cursor to the bottom left of the screen. */

    PDC_gotoyx(PDC_get_rows() - 2, 0);
}

void PDC_scr_free(void)
{
    if (SP)
        free(SP);
    if (pdc_atrtab)
        free(pdc_atrtab);

    pdc_atrtab = (unsigned char *)NULL;
}

/* open the physical screen -- allocate SP, miscellaneous intialization, 
   and may save the existing screen for later restoration */

int PDC_scr_open(int argc, char **argv)
{
    int i;

    PDC_LOG(("PDC_scr_open() - called\n"));

    SP = calloc(1, sizeof(SCREEN));
    pdc_atrtab = calloc(PDC_COLOR_PAIRS * PDC_OFFSET, 1);

    if (!SP || !pdc_atrtab)
        return ERR;

    for (i = 0; i < 16; i++)
        curstoreal[realtocurs[i]] = i;

    SP->orig_attr = FALSE;

    SP->lines = PDC_get_rows();
    SP->cols = PDC_get_columns();

#if CONFIG_SPEAKER
    SP->audible = TRUE;
#endif

    return OK;
}

/* the core of resize_term() */

int PDC_resize_screen(int nlines, int ncols)
{
    PDC_LOG(("PDC_resize_screen() - called. Lines: %d Cols: %d\n",
             nlines, ncols));

    return ERR;
}

void PDC_reset_prog_mode(void)
{
        PDC_LOG(("PDC_reset_prog_mode() - called.\n"));
}

void PDC_reset_shell_mode(void)
{
        PDC_LOG(("PDC_reset_shell_mode() - called.\n"));
}

void PDC_restore_screen_mode(int i)
{
}

void PDC_save_screen_mode(int i)
{
}

void PDC_init_pair(short pair, short fg, short bg)
{
    unsigned char att, temp_bg;
    chtype i;

    fg = curstoreal[fg];
    bg = curstoreal[bg];

    for (i = 0; i < PDC_OFFSET; i++)
    {
        att = fg | (bg << 4);

        if (i & (A_REVERSE >> PDC_ATTR_SHIFT))
            att = bg | (fg << 4);
        if (i & (A_UNDERLINE >> PDC_ATTR_SHIFT))
            att = 1;
        if (i & (A_INVIS >> PDC_ATTR_SHIFT))
        {
            temp_bg = att >> 4;
            att = temp_bg << 4 | temp_bg;
        }
        if (i & (A_BOLD >> PDC_ATTR_SHIFT))
            att |= 8;
        if (i & (A_BLINK >> PDC_ATTR_SHIFT))
            att |= 128;

        pdc_atrtab[pair * PDC_OFFSET + i] = att;
    }
}

int PDC_pair_content(short pair, short *fg, short *bg)
{
    *fg = realtocurs[pdc_atrtab[pair * PDC_OFFSET] & 0x0F];
    *bg = realtocurs[(pdc_atrtab[pair * PDC_OFFSET] & 0xF0) >> 4];

    return OK;
}

bool PDC_can_change_color(void)
{
    return FALSE;
}

int PDC_color_content(short color, short *red, short *green, short *blue)
{
    return ERR;
}

int PDC_init_color(short color, short red, short green, short blue)
{
    return ERR;
}
