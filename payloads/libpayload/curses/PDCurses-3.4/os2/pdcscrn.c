/* Public Domain Curses */

#include "pdcos2.h"

RCSID("$Id: pdcscrn.c,v 1.76 2008/07/14 04:24:51 wmcbrine Exp $")

#ifdef CHTYPE_LONG
# define PDC_OFFSET 32
#else
# define PDC_OFFSET  8
#endif

/* COLOR_PAIR to attribute encoding table. */

unsigned char *pdc_atrtab = (unsigned char *)NULL;

int pdc_font;  /* default font size */

static short curstoreal[16], realtocurs[16] =
{
    COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN, COLOR_RED,
    COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE, COLOR_BLACK + 8,
    COLOR_BLUE + 8, COLOR_GREEN + 8, COLOR_CYAN + 8, COLOR_RED + 8,
    COLOR_MAGENTA + 8, COLOR_YELLOW + 8, COLOR_WHITE + 8
};

#ifdef EMXVIDEO
static unsigned char *saved_screen = NULL;
static int saved_lines = 0;
static int saved_cols = 0;
#else
# ifdef PDCTHUNK
#  ifdef __EMX__
#   define THUNKEDVIO VIOCOLORREG
#  else

typedef struct {
    USHORT cb;
    USHORT type;
    USHORT firstcolorreg;
    USHORT numcolorregs;
    ptr_16 colorregaddr;
} THUNKEDVIO;

#  endif
# endif

static PCH saved_screen = NULL;
static USHORT saved_lines = 0;
static USHORT saved_cols = 0;
static VIOMODEINFO scrnmode;    /* default screen mode  */
static VIOMODEINFO saved_scrnmode[3];
static int saved_font[3];
static bool can_change = FALSE;

static int _get_font(void)
{
    VIOMODEINFO modeInfo = {0};

    modeInfo.cb = sizeof(modeInfo);

    VioGetMode(&modeInfo, 0);
    return (modeInfo.vres / modeInfo.row);
}

static void _set_font(int size)
{
    VIOMODEINFO modeInfo = {0};

    if (pdc_font != size)
    {
        modeInfo.cb = sizeof(modeInfo);

        /* set most parameters of modeInfo */

        VioGetMode(&modeInfo, 0);
        modeInfo.cb = 8;    /* ignore horiz an vert resolution */
        modeInfo.row = modeInfo.vres / size;
        VioSetMode(&modeInfo, 0);
    }

    curs_set(SP->visibility);

    pdc_font = _get_font();
}

#endif

/* close the physical screen -- may restore the screen to its state
   before PDC_scr_open(); miscellaneous cleanup */

void PDC_scr_close(void)
{
    PDC_LOG(("PDC_scr_close() - called\n"));

    if (saved_screen && getenv("PDC_RESTORE_SCREEN"))
    {
#ifdef EMXVIDEO
        v_putline(saved_screen, 0, 0, saved_lines * saved_cols);
#else
        VioWrtCellStr(saved_screen, saved_lines * saved_cols * 2,
            0, 0, (HVIO)NULL);
#endif
        free(saved_screen);
        saved_screen = NULL;
    }

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
#ifdef EMXVIDEO
    int adapter;
#else
    USHORT totchars;
#endif
    int i;
    short r, g, b;

    PDC_LOG(("PDC_scr_open() - called\n"));

    SP = calloc(1, sizeof(SCREEN));
    pdc_atrtab = calloc(PDC_COLOR_PAIRS * PDC_OFFSET, 1);

    if (!SP || !pdc_atrtab)
        return ERR;

    for (i = 0; i < 16; i++)
        curstoreal[realtocurs[i]] = i;

#ifdef EMXVIDEO
    v_init();
#endif
    SP->orig_attr = FALSE;

#ifdef EMXVIDEO
    adapter = v_hardware();
    SP->mono = (adapter == V_MONOCHROME);

    pdc_font = SP->mono ? 14 : (adapter == V_COLOR_8) ? 8 : 12;
#else
    VioGetMode(&scrnmode, 0);
    PDC_get_keyboard_info();

    pdc_font = _get_font();
#endif
    SP->lines = PDC_get_rows();
    SP->cols = PDC_get_columns();

    SP->mouse_wait = PDC_CLICK_PERIOD;
    SP->audible = TRUE;

    /* This code for preserving the current screen */

    if (getenv("PDC_RESTORE_SCREEN"))
    {
        saved_lines = SP->lines;
        saved_cols = SP->cols;

        saved_screen = malloc(2 * saved_lines * saved_cols);

        if (!saved_screen)
        {
            SP->_preserve = FALSE;
            return OK;
        }
#ifdef EMXVIDEO
        v_getline(saved_screen, 0, 0, saved_lines * saved_cols);
#else
        totchars = saved_lines * saved_cols * 2;
        VioReadCellStr((PCH)saved_screen, &totchars, 0, 0, (HVIO)NULL);
#endif
    }

    SP->_preserve = (getenv("PDC_PRESERVE_SCREEN") != NULL);

    can_change = (PDC_color_content(0, &r, &g, &b) == OK);

    return OK;
}

/* the core of resize_term() */

int PDC_resize_screen(int nlines, int ncols)
{
#ifndef EMXVIDEO
    VIOMODEINFO modeInfo = {0};
    USHORT result;
#endif

    PDC_LOG(("PDC_resize_screen() - called. Lines: %d Cols: %d\n",
              nlines, ncols));

#ifdef EMXVIDEO
    return ERR;
#else
    modeInfo.cb = sizeof(modeInfo);

    /* set most parameters of modeInfo */

    VioGetMode(&modeInfo, 0);
    modeInfo.fbType = 1;
    modeInfo.row = nlines;
    modeInfo.col = ncols;
    result = VioSetMode(&modeInfo, 0);

    LINES = PDC_get_rows();
    COLS = PDC_get_columns();

    return (result == 0) ? OK : ERR;
#endif
}

void PDC_reset_prog_mode(void)
{
    PDC_LOG(("PDC_reset_prog_mode() - called.\n"));

#ifndef EMXVIDEO
    PDC_set_keyboard_binary(TRUE);
#endif
}

void PDC_reset_shell_mode(void)
{
    PDC_LOG(("PDC_reset_shell_mode() - called.\n"));

#ifndef EMXVIDEO
    PDC_set_keyboard_default();
#endif
}

#ifndef EMXVIDEO

static bool _screen_mode_equals(VIOMODEINFO *oldmode)
{
    VIOMODEINFO current = {0};

    VioGetMode(&current, 0);

    return ((current.cb == oldmode->cb) &&
            (current.fbType == oldmode->fbType) &&
            (current.color == oldmode->color) && 
            (current.col == oldmode->col) &&
            (current.row == oldmode->row) && 
            (current.hres == oldmode->vres) &&
            (current.vres == oldmode->vres));
}

#endif

void PDC_restore_screen_mode(int i)
{
#ifndef EMXVIDEO
    if (i >= 0 && i <= 2)
    {
        pdc_font = _get_font();
        _set_font(saved_font[i]);

        if (!_screen_mode_equals(&saved_scrnmode[i]))
            if (VioSetMode(&saved_scrnmode[i], 0) != 0)
            {
                pdc_font = _get_font();
                scrnmode = saved_scrnmode[i];
                LINES = PDC_get_rows();
                COLS = PDC_get_columns();
            }
    }
#endif
}

void PDC_save_screen_mode(int i)
{
#ifndef EMXVIDEO
    if (i >= 0 && i <= 2)
    {
        saved_font[i] = pdc_font;
        saved_scrnmode[i] = scrnmode;
    }
#endif
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
    return can_change;
}

int PDC_color_content(short color, short *red, short *green, short *blue)
{
#ifdef PDCTHUNK
    THUNKEDVIO vcr;
    USHORT palbuf[4];
    unsigned char pal[3];
    int rc;

    /* Read single DAC register */

    palbuf[0] = 8;
    palbuf[1] = 0;
    palbuf[2] = curstoreal[color];

    rc = VioGetState(&palbuf, 0);
    if (rc)
        return ERR;

    vcr.cb = sizeof(vcr);
    vcr.type = 3;
    vcr.firstcolorreg = palbuf[3];
    vcr.numcolorregs = 1;
    vcr.colorregaddr = PDCTHUNK(pal);

    rc = VioGetState(&vcr, 0);
    if (rc)
        return ERR;

    /* Scale and store */

    *red = DIVROUND((unsigned)(pal[0]) * 1000, 63);
    *green = DIVROUND((unsigned)(pal[1]) * 1000, 63);
    *blue = DIVROUND((unsigned)(pal[2]) * 1000, 63);

    return OK;
#else
    return ERR;
#endif
}

int PDC_init_color(short color, short red, short green, short blue)
{
#ifdef PDCTHUNK
    THUNKEDVIO vcr;
    USHORT palbuf[4];
    unsigned char pal[3];
    int rc;

    /* Scale */

    pal[0] = DIVROUND((unsigned)red * 63, 1000);
    pal[1] = DIVROUND((unsigned)green * 63, 1000);
    pal[2] = DIVROUND((unsigned)blue * 63, 1000);

    /* Set single DAC register */

    palbuf[0] = 8;
    palbuf[1] = 0;
    palbuf[2] = curstoreal[color];

    rc = VioGetState(&palbuf, 0);
    if (rc)
        return ERR;

    vcr.cb = sizeof(vcr);
    vcr.type = 3;
    vcr.firstcolorreg = palbuf[3];
    vcr.numcolorregs = 1;
    vcr.colorregaddr = PDCTHUNK(pal);

    rc = VioSetState(&vcr, 0);

    return rc ? ERR : OK;
#else
    return ERR;
#endif
}
