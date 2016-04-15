/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2008 Ulf Jordan <jordan@chalmers.se>
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

#include <libpayload-config.h>
#include <libpayload.h>

/*  These are thinly veiled vt100 functions used by curses */

#define VT100_CLEAR       "\e[H\e[J"
/* These defines will fail if you use bold and reverse at the same time.
 * Switching off one of them will switch off both. tinycurses knows about
 * this and does the right thing.
 */
#define VT100_SBOLD       "\e[1m"
#define VT100_EBOLD       "\e[m"
#define VT100_SREVERSE    "\e[7m"
#define VT100_EREVERSE    "\e[m"
#define VT100_CURSOR_ADDR "\e[%d;%dH"
#define VT100_CURSOR_ON   "\e[?25l"
#define VT100_CURSOR_OFF  "\e[?25h"
/* The following smacs/rmacs are actually for xterm; a real vt100 has
   enacs=\E(B\E)0, smacs=^N, rmacs=^O.  */
#define VT100_SMACS       "\e(0"
#define VT100_RMACS       "\e(B"
/* A vt100 doesn't do color, setaf/setab below are from xterm-color. */
#define VT100_SET_COLOR   "\e[3%d;4%dm"

static void serial_putcmd(const char *str)
{
	while (*str)
		serial_putchar(*(str++));
}

void serial_clear(void)
{
	serial_putcmd(VT100_CLEAR);
}

void serial_start_bold(void)
{
	serial_putcmd(VT100_SBOLD);
}

void serial_end_bold(void)
{
	serial_putcmd(VT100_EBOLD);
}

void serial_start_reverse(void)
{
	serial_putcmd(VT100_SREVERSE);
}

void serial_end_reverse(void)
{
	serial_putcmd(VT100_EREVERSE);
}

void serial_start_altcharset(void)
{
	serial_putcmd(VT100_SMACS);
}

void serial_end_altcharset(void)
{
	serial_putcmd(VT100_RMACS);
}

/**
 * Set the foreground and background colors on the serial console.
 *
 * @param fg Foreground color number.
 * @param bg Background color number.
 */
void serial_set_color(short fg, short bg)
{
	char buffer[32];

	snprintf(buffer, sizeof(buffer), VT100_SET_COLOR, fg, bg);
	serial_putcmd(buffer);
}

void serial_set_cursor(int y, int x)
{
	char buffer[32];

	snprintf(buffer, sizeof(buffer), VT100_CURSOR_ADDR, y + 1, x + 1);
	serial_putcmd(buffer);
}

void serial_cursor_enable(int state)
{
	if (state)
		serial_putcmd(VT100_CURSOR_ON);
	else
		serial_putcmd(VT100_CURSOR_OFF);
}
