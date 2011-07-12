/* $Id: ncurses_cfg.h,v 1.8 2006/12/28 01:02:03 wmcbrine Exp $
 *
 * This file is only used with the ncurses test programs.
 *
 * Have ncurses-5.6 unpacked in your $(HOME) (you don't need to build 
 * it), or edit ncurses_testdir appropriately in the Makefile. Configure 
 * and build PDCurses for X11. (Sorry, other ports won't work yet.) 
 * Change to this directory, and:
 *
 * "make ncurses_tests" to start.
 * "make ncurses_clean" when you're done.
 *
 * Builds: bs gdc hanoi knight tclock ncurses
 */

#define NCURSES_MOUSE_VERSION 2

#include "../config.h"
#include <curses.h>

#define ExitProgram exit

#define HAVE_CURSES_VERSION 1
#define HAVE_GETBEGX 1
#define HAVE_GETCURX 1
#define HAVE_GETMAXX 1
#define HAVE_GETNSTR 1
#define HAVE_GETTIMEOFDAY 1
#define HAVE_GETWIN 1
#define HAVE_LIBPANEL 1
#define HAVE_LOCALE_H 1
#define HAVE_NAPMS 1
#define HAVE_PANEL_H 1
#define HAVE_PUTWIN 1
#define HAVE_SLK_COLOR 1
#define HAVE_SLK_INIT 1
#define HAVE_WRESIZE 1

#ifdef PDC_WIDE
# define USE_WIDEC_SUPPORT 1
#endif

/* Fool ncurses.c so it gives us all the tests, and doesn't redefine 
   ACS_ chars
*/

#define NCURSES_VERSION PDCURSES
