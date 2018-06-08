/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2008 Ulf Jordan <jordan@chalmers.se>
 * Copyright (C) 2008-2009 coresystems GmbH
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

/*
 * This is a tiny implementation of the (n)curses library intended to be
 * used in embedded/firmware/BIOS code where no libc or operating system
 * environment is available and code size is very important.
 *
 * Design goals:
 *  - Small object code.
 *  - Self-contained.
 *    - Doesn't require a libc (no glibc/uclibc/dietlibc/klibc/newlib).
 *    - Works without any other external libraries or header files.
 *  - Works without an underlying operating system.
 *    - Doesn't use files, signals, syscalls, ttys, library calls, etc.
 *  - Doesn't do any dynamic memory allocation (no malloc() and friends).
 *     - All data structures are statically allocated.
 *  - Supports standard VGA console (80x25) and serial port console.
 *     - This includes character output and keyboard input over serial.
 *  - Supports beep() through a minimal PC speaker driver.
 *
 * Limitations:
 *  - Only implements a small subset of the (n)curses functions.
 *  - Only implements very few sanity checks (for smaller code).
 *     - Thus: Don't do obviously stupid things in your code.
 *  - Doesn't implement the 'form', 'panel', and 'menu' extensions.
 *  - Only implements C bindings (no C++, Ada95, or others).
 *  - Doesn't include wide character support.
 */

#include "local.h"

#undef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED 1

#define MAX_WINDOWS 3

/* Statically allocate all structures (no malloc())! */
static WINDOW window_list[MAX_WINDOWS];
static int window_count = 0;

// struct ldat foo;
static struct ldat ldat_list[MAX_WINDOWS][SCREEN_Y];
static int ldat_count = 0;

/* One item bigger than SCREEN_X to reserve space for a NUL byte. */
static NCURSES_CH_T linebuf_list[SCREEN_Y * MAX_WINDOWS][SCREEN_X + 1];
static int linebuf_count = 0;

/* Globals */
int COLORS;		/* Currently unused? */
int COLOR_PAIRS = 255;
WINDOW *stdscr;
WINDOW *curscr;
WINDOW *newscr;
int LINES = 25;
int COLS = 80;
int TABSIZE;
int ESCDELAY;
// char ttytype[];
// cchar_t *_nc_wacs;
SCREEN *SP;
chtype acs_map[128];

/* See terminfo(5). */
chtype fallback_acs_map[128] =
	{
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	'>',	'<',	'^',	'v',	' ',
	'#',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	'+',	':',	' ',	' ',	' ',	' ',	'\\',   '#',
	'#',	'#',	'+',	'+',	'+',	'+',	'+',	'~',
	'-',	'-',	'-',	'_',	'+',	'+',	'+',	'+',
	'|',	'<',	'>',	'*',	'!',	'f',	'o',	' ',
	};

#if IS_ENABLED(CONFIG_LP_SERIAL_CONSOLE)
#if IS_ENABLED(CONFIG_LP_SERIAL_ACS_FALLBACK)
chtype serial_acs_map[128];
#else
/* See acsc of vt100. */
chtype serial_acs_map[128] =
	{
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	'`',	'a',	0,	0,	0,	0,	'f',	'g',
	0,	0,	'j',	'k',	'l',	'm',	'n',	'o',
	'p',	'q',	'r',	's',	't',	'u',	'v',	'w',
	'x',	'y',	'z',	'{',	'|',	'}',	'~',	0,
	};
#endif
#endif

#if IS_ENABLED(CONFIG_LP_VIDEO_CONSOLE)
/* See acsc of linux. */
chtype console_acs_map[128] =
	{
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	'\020', '\021', '\030', '\031',	0,
	'\333',	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	'\004',	'\261',	0,	0,	0,	0,	'\370',	'\361',
	'\260',	'\316',	'\331',	'\277',	'\332',	'\300',	'\305',	'~',
	'\304',	'\304',	'\304',	'_',	'\303', '\264', '\301',	'\302',
	'\263',	'\363',	'\362',	'\343',	'\330',	'\234',	'\376',	0,
	};
#endif

// FIXME: Ugly (and insecure!) hack!
char sprintf_tmp[1024];


int curses_flags = (F_ENABLE_CONSOLE | F_ENABLE_SERIAL);

/* Return bit mask for clearing color pair number if given ch has color */
#define COLOR_MASK(ch) (~(attr_t)((ch) & A_COLOR ? A_COLOR : 0))

/* Compute a rendition of the given char correct for the current context. */
static inline NCURSES_CH_T render_char(WINDOW *win, NCURSES_CH_T ch)
{
	/* TODO. */
	return ch;
}

/* Make render_char() visible while still allowing us to inline it below. */
NCURSES_CH_T _nc_render(WINDOW *win, NCURSES_CH_T ch)
{
	return render_char(win, ch);
}

/*
 * Implementations of most functions marked 'implemented' in include/curses.h:
 */

// int baudrate(void) {}
int beep(void)
{
	/* TODO: Flash the screen if beeping fails? */
#if IS_ENABLED(CONFIG_LP_SPEAKER)
	speaker_tone(1760, 500);	/* 1760 == note A6 */
#endif
	return OK;
}
// bool can_change_color(void) {}
int cbreak(void) { /* TODO */ return 0; }
/* D */ int clearok(WINDOW *win, bool flag) { win->_clear = flag; return OK; }
// int color_content(short color, short *r, short *g, short *b) {}
int curs_set(int on)
{
#if IS_ENABLED(CONFIG_LP_SERIAL_CONSOLE)
	if (curses_flags & F_ENABLE_SERIAL) {
		serial_cursor_enable(on);
	}
#endif
#if IS_ENABLED(CONFIG_LP_VIDEO_CONSOLE)
	if (curses_flags & F_ENABLE_CONSOLE) {
		video_console_cursor_enable(on);
	}
#endif

	return OK;
}
// int def_prog_mode(void) {}
// int def_shell_mode(void) {}
// int delay_output(int) {}
// void delscreen(SCREEN *) {}
int delwin(WINDOW *win)
{
	/* TODO: Don't try to delete stdscr. */
	/* TODO: Don't delete parent windows before subwindows. */

	// if (win->_flags & _SUBWIN)
	// 	touchwin(win->_parent);
	// else if (curscr != 0)
	// 	touchwin(curscr);

	// return _nc_freewin(win);
	return OK;
}
WINDOW *derwin(WINDOW *orig, int num_lines, int num_columns, int begy, int begx)
{
#if 0
	WINDOW *win = NULL;
	int i;
	int flags = _SUBWIN;

	/* Make sure window fits inside the original one. */
	if (begy < 0 || begx < 0 || orig == 0 || num_lines < 0
	    || num_columns < 0)
		return NULL;

	if (begy + num_lines > orig->_maxy + 1
	    || begx + num_columns > orig->_maxx + 1)
		return NULL;

	if (num_lines == 0)
		num_lines = orig->_maxy + 1 - begy;

	if (num_columns == 0)
		num_columns = orig->_maxx + 1 - begx;

	if (orig->_flags & _ISPAD)
		flags |= _ISPAD;

	// FIXME
	if ((win = _nc_makenew(num_lines, num_columns, orig->_begy + begy,
	                        orig->_begx + begx, flags)) == 0)
	     return NULL;

	win->_pary = begy;
	win->_parx = begx;
	WINDOW_ATTRS(win) = WINDOW_ATTRS(orig);
	win->_nc_bkgd = orig->_nc_bkgd;

	for (i = 0; i < num_lines; i++)
		win->_line[i].text = &orig->_line[begy++].text[begx];

	win->_parent = orig;

	return win;
#else
	return NULL;
#endif
}
int doupdate(void) { /* TODO */ return(0); }
// WINDOW * dupwin (WINDOW *) {}
/* D */ int echo(void) { SP->_echo = TRUE; return OK; }
int endwin(void)
{
	if (!SP)
		return ERR;

	SP->_endwin = TRUE;
#ifdef NCURSES_MOUSE_VERSION
	SP->_mouse_wrap(SP);
#endif
	// _nc_screen_wrap();
	// _nc_mvcur_wrap();       /* wrap up cursor addressing */
	// return reset_shell_mode();
	return OK;	// FIXME
}
// char erasechar (void) {}
// void filter (void) {}
// int flash(void) {}
int flushinp(void) { /* TODO */ return 0; }
// WINDOW *getwin (FILE *) {}
bool has_colors (void) { return(TRUE); }
// bool has_ic (void) {}
// bool has_il (void) {}
// void idcok (WINDOW *, bool) {}
// int idlok (WINDOW *, bool) {}
void immedok(WINDOW *win, bool flag) { win->_immed = flag; }
/** Note: Must _not_ be called twice! */
WINDOW *initscr(void)
{
	int i;

	// newterm(name, stdout, stdin);
	// def_prog_mode();

	for (i = 0; i < 128; i++)
	  acs_map[i] = (chtype) i | A_ALTCHARSET;
#if IS_ENABLED(CONFIG_LP_SERIAL_CONSOLE)
	if (curses_flags & F_ENABLE_SERIAL) {
		serial_clear();
	}
#endif
#if IS_ENABLED(CONFIG_LP_VIDEO_CONSOLE)
	if (curses_flags & F_ENABLE_CONSOLE) {
		/* Clear the screen and kill the cursor */

		video_console_clear();
		video_console_cursor_enable(0);
	}
#endif

	// Speaker init?

	stdscr = newwin(SCREEN_Y, SCREEN_X, 0, 0);
	// TODO: curscr, newscr?

	werase(stdscr);

	return stdscr;
}
// int intrflush (WINDOW *,bool) {}
/* D */ bool isendwin(void) { return ((SP == NULL) ? FALSE : SP->_endwin); }
// bool is_linetouched (WINDOW *,int) {}
// bool is_wintouched (WINDOW *) {}
// NCURSES_CONST char * keyname (int) {}
int keypad (WINDOW *win, bool flag) { /* TODO */ return 0; }
// char killchar (void) {}
/* D */ int leaveok(WINDOW *win, bool flag) { win->_leaveok = flag; return OK; }
// char *longname (void) {}
// int meta (WINDOW *,bool) {}
// int mvcur (int,int,int,int) {}
// int mvderwin (WINDOW *, int, int) {}
int mvprintw(int y, int x, const char *fmt, ...)
{
	va_list argp;
	int code;

	if (move(y, x) == ERR)
		return ERR;

	va_start(argp, fmt);
	code = vwprintw(stdscr, fmt, argp);
	va_end(argp);

	return code;
}
// int mvscanw (int,int, NCURSES_CONST char *,...) {}
// int mvwin (WINDOW *,int,int) {}
int mvwprintw(WINDOW *win, int y, int x, const char *fmt, ...)
{
	va_list argp;
	int code;

	if (wmove(win, y, x) == ERR)
		return ERR;

	va_start(argp, fmt);
	code = vwprintw(win, fmt, argp);
	va_end(argp);

	return code;
}
// int mvwscanw (WINDOW *,int,int, NCURSES_CONST char *,...) {}
// int napms (int) {}
// WINDOW *newpad (int,int) {}
// SCREEN *newterm (NCURSES_CONST char *,FILE *,FILE *) {}
WINDOW *newwin(int num_lines, int num_columns, int begy, int begx)
{
	WINDOW *win;
	int i;

	/* Use next statically allocated window. */
	// TODO: Error handling. Yes. Please.
	// TODO: WINDOWLIST?

	if (window_count >= MAX_WINDOWS)
		return NULL;

	win = &window_list[window_count++];

	// bool is_pad = (flags & _ISPAD);

	// TODO: Checks.

	win->_cury = 0;
	win->_curx = 0;
	win->_maxy = num_lines - 1;
	win->_maxx = num_columns - 1;
	win->_begy = begy;
	win->_begx = begx;
	// win->_yoffset = SP->_topstolen;

	win->_line = ldat_list[ldat_count++];

	/* FIXME: Is this right? Should the window attributes be normal? */
	win->_color = PAIR_NUMBER(0);
	win->_attrs = A_NORMAL;

	for (i = 0; i < num_lines; i++)
		win->_line[i].text =
		     (NCURSES_CH_T *)&linebuf_list[linebuf_count++];

	return win;
}
/* D */ int nl(void) { SP->_nl = TRUE; return OK; }
/* D */ int noecho(void) { SP->_echo = FALSE; return OK; }
/* D */ int nonl(void) { SP->_nl = FALSE; return OK; }
// void noqiflush (void) {}
// int noraw (void) {}
/* D */ int notimeout (WINDOW *win, bool f) { win->_notimeout = f; return OK; }
// int overlay (const WINDOW*,WINDOW *) {}
// int overwrite (const WINDOW*,WINDOW *) {}
// int pair_content (short,short*,short*) {}
// int pechochar (WINDOW *, const chtype) {}
// int pnoutrefresh (WINDOW*,int,int,int,int,int,int) {}
// int prefresh (WINDOW *,int,int,int,int,int,int) {}
int printw(const char *fmt, ...)
{
	va_list argp;
	int code;

	va_start(argp, fmt);
	code = vwprintw(stdscr, fmt, argp);
	va_end(argp);

	return code;
}
// int putwin (WINDOW *, FILE *) {}
// void qiflush (void) {}
// int raw (void) {}
// int resetty (void) {}
// int reset_prog_mode (void) {}
// int reset_shell_mode (void) {}
// int ripoffline (int, int (*)(WINDOW *, int)) {}
// int savetty (void) {}
// int scanw (NCURSES_CONST char *,...) {}
// int scr_dump (const char *) {}
// int scr_init (const char *) {}
/* D */ int scrollok(WINDOW *win, bool flag) { win->_scroll = flag; return OK; }
// int scr_restore (const char *) {}
// int scr_set (const char *) {}
// SCREEN *set_term (SCREEN *) {}
// int slk_attroff (const chtype) {}
// int slk_attron (const chtype) {}
// int slk_attrset (const chtype) {}
// attr_t slk_attr (void) {}
// int slk_attr_set (const attr_t,short,void*) {}
// int slk_clear (void) {}
// int slk_color (short) {}
// int slk_init (int) {}
/* D */ char *slk_label(int n)
{
	// TODO: Needed?
	// if (SP == NULL || SP->_slk == NULL || n < 1 || n > SP->_slk->labcnt)
	// 	return NULL;
	return SP->_slk->ent[n - 1].ent_text;
}
// int slk_noutrefresh (void) {}
// int slk_refresh (void) {}
// int slk_restore (void) {}
// int slk_set (int,const char *,int) {}
// int slk_touch (void) {}

// WINDOW *subpad (WINDOW *, int, int, int, int) {}
WINDOW *subwin(WINDOW *w, int l, int c, int y, int x)
{
	return derwin(w, l, c, y - w->_begy, x - w->_begx);
}
// int syncok (WINDOW *, bool) {}
// chtype termattrs (void) {}
// char *termname (void) {}
// int typeahead (int) {}
int ungetch(int ch) { /* TODO */ return ERR; }
// void use_env (bool) {}
// int vidattr (chtype) {}
// int vidputs (chtype, int (*)(int)) {}
int vwprintw(WINDOW *win, const char *fmt, va_list argp)
{
	vsprintf((char *)&sprintf_tmp, fmt, argp);

	/* TODO: Error handling? */
	return waddstr(win, (char *)&sprintf_tmp);
}
// int vwscanw (WINDOW *, NCURSES_CONST char *,va_list) {}
int waddch(WINDOW *win, const chtype ch)
{
	int code = ERR;
	// NCURSES_CH_T wch;
	// SetChar2(wch, ch);

	if (win->_line[win->_cury].firstchar == _NOCHANGE ||
			win->_line[win->_cury].firstchar > win->_curx)
		win->_line[win->_cury].firstchar = win->_curx;

	win->_line[win->_cury].text[win->_curx].chars[0] =
		((ch) & (chtype)A_CHARTEXT);

	win->_line[win->_cury].text[win->_curx].attr = WINDOW_ATTRS(win);
	win->_line[win->_cury].text[win->_curx].attr |=
		((ch) & (chtype)A_ATTRIBUTES);

	if (win->_line[win->_cury].lastchar == _NOCHANGE ||
			win->_line[win->_cury].lastchar < win->_curx)
		win->_line[win->_cury].lastchar = win->_curx;

	win->_curx++;	// FIXME

	// if (win && (waddch_nosync(win, wch) != ERR)) {
	// 	_nc_synchook(win);
	// 	code = OK;
	// }

	return code;
}
// int waddchnstr (WINDOW *,const chtype *,int) {}
int waddnstr(WINDOW *win, const char *astr, int n)
{
	int code = OK;
	const char *str = astr;

	if (!str)
		return ERR;

	if (n < 0)
		n = strlen(astr);

	if (win->_line[win->_cury].firstchar == _NOCHANGE ||
			win->_line[win->_cury].firstchar > win->_curx)
		win->_line[win->_cury].firstchar = win->_curx;

	while ((n-- > 0) && (*str != '\0')) {
	// while (*str != '\0') {
		win->_line[win->_cury].text[win->_curx].chars[0] = *str++;
		win->_line[win->_cury].text[win->_curx].attr = WINDOW_ATTRS(win)
;
		win->_curx++;	// FIXME

		// NCURSES_CH_T ch;
		// SetChar(ch, UChar(*str++), A_NORMAL);
		// if (_nc_waddch_nosync(win, ch) == ERR) {
		// 	code = ERR;
		// 	break;
		// }
	}

	if (win->_line[win->_cury].lastchar == _NOCHANGE ||
			win->_line[win->_cury].lastchar < win->_curx)
		win->_line[win->_cury].lastchar = win->_curx;

	return code;
}
int wattr_on(WINDOW *win, attr_t at, void *opts GCC_UNUSED)
{
	if (at & A_COLOR)
		win->_color = PAIR_NUMBER(at);
	// toggle_attr_on(WINDOW_ATTRS(win), at);
	return OK;
}
int wattr_off(WINDOW *win, attr_t at, void *opts GCC_UNUSED)
{
	if (at & A_COLOR)
		win->_color = 0;
	// toggle_attr_off(WINDOW_ATTRS(win), at);
	return 0;
}
// int wbkgd (WINDOW *, chtype) {}
void wbkgdset(WINDOW *win, chtype ch) { /* TODO */ }

int wborder(WINDOW *win, chtype ls, chtype rs, chtype ts, chtype bs,
		chtype tl, chtype tr, chtype bl, chtype br)
{
	int x, y;

	if (ls == 0) ls = ACS_VLINE;
	if (rs == 0) rs = ACS_VLINE;
	if (ts == 0) ts = ACS_HLINE;
	if (bs == 0) bs = ACS_HLINE;
	if (tl == 0) tl = ACS_ULCORNER;
	if (tr == 0) tr = ACS_URCORNER;
	if (bl == 0) bl = ACS_LLCORNER;
	if (br == 0) br = ACS_LRCORNER;

	for(y = 0; y <= win->_maxy; y++) {

		if (y == 0) {
			 mvwaddch(win, y, 0, tl);

			for(x = 1; x < win->_maxx; x++)
				mvwaddch(win, y, x, ts);

			mvwaddch(win, y, win->_maxx, tr);
		}
		else if (y == win->_maxy) {
			mvwaddch(win, y, 0, bl);

			for(x = 1; x < win->_maxx; x++)
				mvwaddch(win, y, x, bs);

			mvwaddch(win, y, win->_maxx, br);
		}
		else {
			mvwaddch(win, y, 0, ls);
			mvwaddch(win, y, win->_maxx, rs);
		}
	}

	return OK;
}

// int wchgat (WINDOW *, int, attr_t, short, const void *) {}
/* D */ int wclear(WINDOW *win)
{
	if (werase(win) == ERR)
		return ERR;
	win->_clear = TRUE;
	return OK;
}
// int wclrtobot (WINDOW *) {}
int wclrtoeol(WINDOW *win) { /* TODO */ return ERR; }
int wcolor_set(WINDOW *win, short color_pair_number, void *opts)
{
	if (!opts && (color_pair_number >= 0)
	    && (color_pair_number < COLOR_PAIRS)) {
		SET_WINDOW_PAIR(win, color_pair_number);
		if_EXT_COLORS(win->_color = color_pair_number);
		return OK;
	}
	return ERR;
}
// void wcursyncup (WINDOW *) {}
// int wdelch (WINDOW *) {}
// int wechochar (WINDOW *, const chtype) {}
int werase(WINDOW *win)
{
	int x, y;
	for (y = 0; y <= win->_maxy; y++) {
		for (x = 0; x <= win->_maxx; x++) {
			win->_line[y].text[x].chars[0] = ' ';
			win->_line[y].text[x].attr = WINDOW_ATTRS(win);
		}
		// Should we check instead?
		win->_line[y].firstchar = 0;
		win->_line[y].lastchar = win->_maxx;
	}
	return OK;
}
// int wgetnstr (WINDOW *,char *,int) {}
int whline(WINDOW *win, chtype ch, int n)
{
	NCURSES_SIZE_T start, end;
	struct ldat *line = &(win->_line[win->_cury]);
	NCURSES_CH_T wch;

	start = win->_curx;
	end = start + n - 1;
	if (end > win->_maxx)
		end = win->_maxx;

	CHANGED_RANGE(line, start, end);

	//// TODO:
	//// if (ch == 0)
	////     SetChar2(wch, ACS_HLINE);
	//// else
	////     SetChar2(wch, ch);
	// Ugly hack:
	wch.chars[0] = ((ch) & (chtype)A_CHARTEXT);
	wch.attr = ((ch) & (chtype)A_ATTRIBUTES);
	wch = _nc_render(win, wch);

	while (end >= start) {
		line->text[end] = wch;
		end--;
	}

	//// _nc_synchook(win);

	return OK;
}
/* D */ chtype winch(WINDOW *win)
{
	//// TODO
	// return (CharOf(win->_line[win->_cury].text[win->_curx]) |
	//         AttrOf(win->_line[win->_cury].text[win->_curx]));
	return OK;	// FIXME
}
// int winchnstr (WINDOW *, chtype *, int) {}
// int winnstr (WINDOW *, char *, int) {}
// int winsch (WINDOW *, chtype) {}
// int winsdelln (WINDOW *,int) {}
// int winsnstr (WINDOW *, const char *,int) {}
/* D */ int wmove(WINDOW *win, int y, int x)
{
	if (!LEGALYX(win, y, x))
		return ERR;
	win->_curx = (NCURSES_SIZE_T) x;
	win->_cury = (NCURSES_SIZE_T) y;
	win->_flags &= ~_WRAPPED;
	win->_flags |= _HASMOVED;
	return OK;
}

#define SWAP_RED_BLUE(c) \
	(((c) & 0x4400) >> 2) | ((c) & 0xAA00) | (((c) & 0x1100) << 2)
int wnoutrefresh(WINDOW *win)
{
#if IS_ENABLED(CONFIG_LP_SERIAL_CONSOLE)
	// FIXME.
	int serial_is_bold = 0;
	int serial_is_reverse = 0;
	int serial_is_altcharset = 0;
	int serial_cur_pair = 0;

	int need_altcharset;
	short fg, bg;
#endif
	int x, y;
	chtype ch;

#if IS_ENABLED(CONFIG_LP_SERIAL_CONSOLE)
	serial_end_bold();
	serial_end_altcharset();
#endif

	for (y = 0; y <= win->_maxy; y++) {

		if (win->_line[y].firstchar == _NOCHANGE)
			continue;

		/* Position the serial cursor */

#if IS_ENABLED(CONFIG_LP_SERIAL_CONSOLE)
		if (curses_flags & F_ENABLE_SERIAL)
			serial_set_cursor(win->_begy + y, win->_begx +
					win->_line[y].firstchar);
#endif

		for (x = win->_line[y].firstchar; x <= win->_line[y].lastchar; x++) {
			attr_t attr = win->_line[y].text[x].attr;

#if IS_ENABLED(CONFIG_LP_SERIAL_CONSOLE)
			if (curses_flags & F_ENABLE_SERIAL) {
				ch = win->_line[y].text[x].chars[0];

				if (attr & A_BOLD) {
					if (!serial_is_bold) {
						serial_start_bold();
						serial_is_bold = 1;
					}
				} else {
					if (serial_is_bold) {
						serial_end_bold();
						serial_is_bold = 0;
						/* work around serial.c
						 * shortcoming:
						 */
						serial_is_reverse = 0;
						serial_cur_pair = 0;
					}
				}

				if (attr & A_REVERSE) {
					if (!serial_is_reverse) {
						serial_start_reverse();
						serial_is_reverse = 1;
					}
				} else {
					if (serial_is_reverse) {
						serial_end_reverse();
						serial_is_reverse = 0;
						/* work around serial.c
						 * shortcoming:
						 */
						serial_is_bold = 0;
						serial_cur_pair = 0;
					}
				}

				need_altcharset = 0;
				if (attr & A_ALTCHARSET) {
					if (serial_acs_map[ch & 0x7f]) {
						ch = serial_acs_map[ch & 0x7f];
						need_altcharset = 1;
					} else
						ch = fallback_acs_map[ch & 0x7f];
				}
				if (need_altcharset && !serial_is_altcharset) {
					serial_start_altcharset();
					serial_is_altcharset = 1;
				}
				if (!need_altcharset && serial_is_altcharset) {
					serial_end_altcharset();
					serial_is_altcharset = 0;
				}

				if (serial_cur_pair != PAIR_NUMBER(attr)) {
					pair_content(PAIR_NUMBER(attr),
						     &fg, &bg);
					serial_set_color(fg, bg);
					serial_cur_pair = PAIR_NUMBER(attr);
				}

				serial_putchar(ch);

			}
#endif
#if IS_ENABLED(CONFIG_LP_VIDEO_CONSOLE)
			unsigned int c =
				((int)color_pairs[PAIR_NUMBER(attr)]) << 8;

			c = SWAP_RED_BLUE(c);

			if (curses_flags & F_ENABLE_CONSOLE) {
				ch = win->_line[y].text[x].chars[0];

				/* Handle some of the attributes. */
				if (attr & A_BOLD)
					c |= 0x0800;
				if (attr & A_DIM)
					c &= ~0x800;
				if (attr & A_REVERSE) {
					unsigned char tmp = (c >> 8) & 0xf;
					c = (c >> 4) & 0xf00;
					c |= tmp << 12;
				}
				if (attr & A_ALTCHARSET) {
					if (console_acs_map[ch & 0x7f])
						ch = console_acs_map[ch & 0x7f];
					else
						ch = fallback_acs_map[ch & 0x7f];
				}

				/*
				 * FIXME: Somewhere along the line, the
				 * character value is getting sign-extented.
				 * For now grab just the 8 bit character,
				 * but this will break wide characters!
				 */
				c |= (chtype) (ch & 0xff);
				video_console_putc(win->_begy + y, win->_begx + x, c);
			}
#endif
		}
		win->_line[y].firstchar = _NOCHANGE;
		win->_line[y].lastchar = _NOCHANGE;
	}

#if IS_ENABLED(CONFIG_LP_SERIAL_CONSOLE)
	if (curses_flags & F_ENABLE_SERIAL)
		serial_set_cursor(win->_begy + win->_cury, win->_begx + win->_curx);
#endif

#if IS_ENABLED(CONFIG_LP_VIDEO_CONSOLE)
	if (curses_flags & F_ENABLE_CONSOLE)
		video_console_set_cursor(win->_begx + win->_curx, win->_begy + win->_cury);
#endif

	return OK;
}
int wprintw(WINDOW *win, const char *fmt, ...)
{
	va_list argp;
	int code;

	va_start(argp, fmt);
	code = vwprintw(win, fmt, argp);
	va_end(argp);

	return code;
}

int wredrawln (WINDOW *win, int beg_line, int num_lines)
{
	int i;

	for (i = beg_line; i < beg_line + num_lines; i++) {
		win->_line[i].firstchar = 0;
		win->_line[i].lastchar = win->_maxx;
	}

	return OK;
}

int wrefresh(WINDOW *win)
{
	// FIXME
	return wnoutrefresh(win);

	// XXX
	int code;

	if (win == curscr) {
		curscr->_clear = TRUE;
		// code = doupdate();
	} else if ((code = wnoutrefresh(win)) == OK) {
		if (win->_clear)
			newscr->_clear = TRUE;
		// code = doupdate();
		/*
		 * Reset the clearok() flag in case it was set for the special
		 * case in hardscroll.c (if we don't reset it here, we'll get 2
		 * refreshes because the flag is copied from stdscr to newscr).
		 * Resetting the flag shouldn't do any harm, anyway.
		 */
		win->_clear = FALSE;
	}

	return code;
}
// int wscanw (WINDOW *, NCURSES_CONST char *,...) {}
int wscrl(WINDOW *win, int n)
{
	int x, y;

	if (!win->_scroll)
		return ERR;

	if (n == 0)
		return OK;

	for (y = 0; y <= (win->_maxy - n); y++) {
		win->_line[y].firstchar = win->_line[y + n].firstchar;
		win->_line[y].lastchar = win->_line[y + n].lastchar;
		for (x = 0; x <= win->_maxx; x++) {
			if ((win->_line[y].text[x].chars[0] != win->_line[y + n].text[x].chars[0]) ||
					(win->_line[y].text[x].attr != win->_line[y + n].text[x].attr)) {
				if (win->_line[y].firstchar == _NOCHANGE)
					win->_line[y].firstchar = x;

				win->_line[y].lastchar = x;

				win->_line[y].text[x].chars[0] = win->_line[y + n].text[x].chars[0];
				win->_line[y].text[x].attr = win->_line[y + n].text[x].attr;
			}
		}
	}

	for (y = (win->_maxy+1 - n); y <= win->_maxy; y++) {
		for (x = 0; x <= win->_maxx; x++) {
			if ((win->_line[y].text[x].chars[0] != ' ') ||
					(win->_line[y].text[x].attr != A_NORMAL)) {
				if (win->_line[y].firstchar == _NOCHANGE)
					win->_line[y].firstchar = x;

				win->_line[y].lastchar = x;

				win->_line[y].text[x].chars[0] = ' ';
				win->_line[y].text[x].attr = A_NORMAL;
			}
		}
	}

	// _nc_scroll_window(win, n, win->_regtop, win->_regbottom, win->_nc_bkgd);
	// _nc_synchook(win);

	return OK;
}
int wsetscrreg(WINDOW *win, int top, int bottom)
{
	if (top >= 0 && top <= win->_maxy && bottom >= 0 &&
	    bottom <= win->_maxy && bottom > top) {
		win->_regtop = (NCURSES_SIZE_T) top;
		win->_regbottom = (NCURSES_SIZE_T) bottom;
		return OK;
	}
	return ERR;
}
// void wsyncdown (WINDOW *) {}
// void wsyncup (WINDOW *) {}
/* D */ void wtimeout(WINDOW *win, int _delay) { win->_delay = _delay; }
/* D */ int wtouchln(WINDOW *win, int y, int n, int changed)
{
	int i;

	// if ((n < 0) || (y < 0) || (y > win->_maxy))
	//     return ERR;

	for (i = y; i < y + n; i++) {
		if (i > win->_maxy)
			break;
		win->_line[i].firstchar = changed ? 0 : _NOCHANGE;
		win->_line[i].lastchar = changed ? win->_maxx : _NOCHANGE;
	}
	return OK;
}
// int wvline (WINDOW *,chtype,int) {}
// int tigetflag (NCURSES_CONST char *) {}
// int tigetnum (NCURSES_CONST char *) {}
// char *tigetstr (NCURSES_CONST char *) {}
// int putp (const char *) {}
// #if NCURSES_TPARM_VARARGS
// char *tparm (NCURSES_CONST char *, ...) {}
// #else
// char *tparm (NCURSES_CONST char *, long,long,long,long,long,long,long,long,long) {}
// char *tparm_varargs (NCURSES_CONST char *, ...) {}
// #endif
