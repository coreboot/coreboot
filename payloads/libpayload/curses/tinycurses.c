/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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
 *  - Doesn't implement the 'form', 'panel', and 'menu' extentions.
 *  - Only implements C bindings (no C++, Ada95, or others).
 *  - Doesn't include wide character support.
 */

#include "local.h"

#undef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED 1

#define MAX_WINDOWS 3

/* Statically allocate all structures (no malloc())! */
static WINDOW window_list[MAX_WINDOWS];
static int window_count = 1;

// struct ldat foo;
static struct ldat ldat_list[3];
static int ldat_count = 0;

/* One item bigger than SCREEN_X to reverse place for a NUL byte. */
static NCURSES_CH_T linebuf_list[SCREEN_Y * MAX_WINDOWS][SCREEN_X + 1];
static int linebuf_count = 0;

/* Globals */
int COLORS;		/* Currently unused? */
int COLOR_PAIRS;
WINDOW *stdscr;
WINDOW *curscr;
WINDOW *newscr;
int LINES;
int COLS;
int TABSIZE;
int ESCDELAY;
// char ttytype[];
// cchar_t *_nc_wacs;
SCREEN *SP;
chtype acs_map[128];

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
 * Implementations of most functions marked 'implemented' in tinycurses.h:
 */

// int baudrate(void) {}
int beep(void)
{
	/* TODO: Flash the screen if beeping fails? */
	speaker_tone(1760, 500);	/* 1760 == note A6 */
	return OK;
}
// bool can_change_color(void) {}
int cbreak(void) { /* TODO */ return 0; }
/* D */ int clearok(WINDOW *win, bool flag) { win->_clear = flag; return OK; }
// int color_content(short color, short *r, short *g, short *b) {}
// int curs_set(int) {}
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
	//// if ((win = _nc_makenew(num_lines, num_columns, orig->_begy + begy,
	////                        orig->_begx + begx, flags)) == 0)
	////     return NULL;

	win->_pary = begy;
	win->_parx = begx;
	WINDOW_ATTRS(win) = WINDOW_ATTRS(orig);
	win->_nc_bkgd = orig->_nc_bkgd;

	for (i = 0; i < num_lines; i++)
		win->_line[i].text = &orig->_line[begy++].text[begx];

	win->_parent = orig;

	return win;
}
int doupdate(void) { /* TODO */ return(*(int *)0); }
// WINDOW * dupwin (WINDOW *) {}
/* D */ int echo(void) { SP->_echo = TRUE; return OK; }
int endwin(void)
{
	if (!SP)
		return ERR;

	SP->_endwin = TRUE;
	SP->_mouse_wrap(SP);
	// _nc_screen_wrap();
	// _nc_mvcur_wrap();       /* wrap up cursor addressing */
	// return reset_shell_mode();
	return OK;	// FIXME
}
// char erasechar (void) {}
// void filter (void) {}
// int flash(void) {}
// int flushinp (void) {}
// WINDOW *getwin (FILE *) {}

bool has_colors (void) { /* TODO */ return(*(bool *)0); }
// bool has_ic (void) {}
// bool has_il (void) {}
// void idcok (WINDOW *, bool) {}
// int idlok (WINDOW *, bool) {}
void immedok(WINDOW *win, bool flag) { win->_immed = flag; }
/** Note: Must _not_ be called twice! */
WINDOW *initscr(void)
{
	int x, y;

	// newterm(name, stdout, stdin);
	// def_prog_mode();

	if (curses_flags & F_ENABLE_CONSOLE) {
		/* Clear the screen and kill the cursor. */
		vga_clear();
		vga_cursor_enable(0);
	}

	// Speaker init?

	stdscr = newwin(SCREEN_Y, SCREEN_X + 1, 0, 0);
	// TODO: curscr, newscr?

	for (y = 0; y < stdscr->_maxy; y++) {
		for (x = 0; x < stdscr->_maxx; x++) {
			stdscr->_line[y].text[x].chars[0] = ' ';
			stdscr->_line[y].text[x].attr = A_NORMAL;
		}
	}

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
	int i;

	/* Use next statically allocated window. */
	// TODO: Error handling.
	// TODO: WINDOWLIST?
	WINDOW *win = &window_list[window_count++];

	// bool is_pad = (flags & _ISPAD);

	// TODO: Checks.

	win->_cury = 0;
	win->_curx = 0;
	win->_maxy = num_lines - 1;
	win->_maxx = num_columns - 1;
	win->_begy = begy;
	win->_begx = begx;
	// win->_yoffset = SP->_topstolen;

	win->_line = &ldat_list[ldat_count++];

	/* FIXME: Is this right? Should the window attributes be normal? */
	win->_color = PAIR_NUMBER(0);
	win->_attrs = A_NORMAL;

	for (i = 0; i < num_lines; i++)
		win->_line[i].text =
		     (NCURSES_CH_T *)&linebuf_list[linebuf_count++];

	return win;
}
/* D */ int nl(void) { SP->_nl = TRUE; return OK; }
int nocbreak(void) { /* TODO */ return(*(int *)0); }

/* D */ int noecho(void) { SP->_echo = FALSE; return OK; }
/* D */ int nonl(void) { SP->_nl = FALSE; return OK; }
// void noqiflush (void) {}
// int noraw (void) {}
// int notimeout (WINDOW *,bool) {}
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
// int waddch (WINDOW *, const chtype) {}
int waddch(WINDOW *win, const chtype ch)
{
	int code = ERR;
	// NCURSES_CH_T wch;
	// SetChar2(wch, ch);

	win->_line[win->_cury].text[win->_curx].chars[0] = ch;

	/* Use the window attributes - perhaps we also pull attributes from
	   the ch itself, I don't know */

	win->_line[win->_cury].text[win->_curx].attr = WINDOW_ATTRS(win);
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
// int wborder (WINDOW *,chtype,chtype,chtype,chtype,chtype,chtype,chtype,chtype) {}
// int wchgat (WINDOW *, int, attr_t, short, const void *) {}
/* D */ int wclear(WINDOW *win)
{
	if (werase(win) == ERR)
		return ERR;
	win->_clear = TRUE;
	return OK;
}
// int wclrtobot (WINDOW *) {}
int wclrtoeol(WINDOW *win) { /* TODO */ return(*(int *)0); }
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
	for (y = 0; y < win->_maxy; y++) {
		for (x = 0; x < win->_maxx; x++) {
			win->_line[y].text[x].chars[0] = ' ';
			win->_line[y].text[x].attr = WINDOW_ATTRS(win);
		}
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


int wnoutrefresh(WINDOW *win)
{
	// FIXME.
	int x, y;

	for (y = 0; y < win->_maxy; y++) {
		for (x = 0; x < win->_maxx; x++) {
			if (curses_flags & F_ENABLE_SERIAL)
				serial_putchar(win->_line[y].text[x].chars[0]);

			if (curses_flags & F_ENABLE_CONSOLE) {
				attr_t attr = win->_line[y].text[x].attr;
				unsigned int c =
				  ((int)color_pairs[PAIR_NUMBER(attr)]) << 8;

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

				c |= win->_line[y].text[x].chars[0];
				vga_putc(y, x, c);
			}
		}
	}

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
// int wredrawln (WINDOW *,int,int) {}
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
	if (!win->_scroll)
		return ERR;

	if (n != 0) {
		// _nc_scroll_window(win, n, win->_regtop, win->_regbottom, win->_nc_bkgd);
		// _nc_synchook(win);
	}
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
// void wtimeout (WINDOW *,int) {}
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
