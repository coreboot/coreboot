/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
 * This file handles reading keystrokes from serial and the console
 * and "cooking" them so that they are correct for curses.
 * Also, implement key related functions (mainly wgetch)
 *
 * TODO:
 * Actually cook the serial (handle special keys)
 */

#include "local.h"

static int _halfdelay = 0;

/* ============== Serial ==================== */

/* FIXME:  Cook the serial correctly */

static int cook_serial(unsigned char ch)
{
	return (int) ch;
}

/* ================ Keyboard ================ */

/* Scancode macros */

#define DOWN(_c) (0x80 | (_c))
#define UP(_c) (_c)

#define ISDOWN(_c) ((_c) & 0x80)
#define ISUP(_c) (!ISDOWN((_c)))

#define SCANCODE(_c) ((_c) & ~0x80)

/* Scancode definitions for the modifiers */

#define SCANCODE_RSHIFT   0x36
#define SCANCODE_LSHIFT   0x2a
#define SCANCODE_CAPSLOCK 0x3a
#define SCANCODE_LALT     0x38
#define SCANCODE_LCTRL    0x1d

/* Modifier flags */

#define SHIFT_MODIFIER    0x1
#define CAPSLOCK_MODIFIER 0x2
#define ALT_MODIFIER      0x4
#define CTRL_MODIFIER     0x8

#define CTRL(_c) (_c & 0x1f)

struct {
	int normal;
	int shift;
} scancode_map[] = {
	{ },
	{ CTRL('['), CTRL('[')}, 
	{ '1', '!' },
	{ '2', '@' },
	{ '3', '#' },
	{ '4', '$' },
	{ '5', '%' },
	{ '6', '^' },
	{ '7', '&' },
	{ '8', '*' },
	{ '9', '(' },
	{ '0', ')' },
	{ '-', '_' },
	{ '=', '+' },
	{ KEY_BACKSPACE, KEY_BACKSPACE},
	{ CTRL('I' ), KEY_BTAB },          /* 0x0F */
	{ 'q', 'Q' },
	{ 'w', 'W' },
	{ 'e', 'E' },
	{ 'r', 'R' },
	{ 't', 'T' },
	{ 'y', 'Y' },
	{ 'u', 'U' },
	{ 'i', 'I' },
	{ 'o', 'O' },
	{ 'p', 'P' },
	{ '[', '{' },
	{ ']', '{' },
	{ KEY_ENTER, KEY_ENTER },
	{ 0 , 0 },
	{ 'a', 'A' },
	{ 's', 'S' },                    /* 0x1F */
	{ 'd', 'D' },
	{ 'f', 'F' },
	{ 'g', 'G' },
	{ 'h', 'H' },
	{ 'j', 'J' },
	{ 'k', 'K' },
	{ 'l', 'L' },
	{ ';', ':' },
	{ '\'', '\"' },
	{ '`', '~', },
	{ 0, 0 },
	{ '\\', '|' },
	{ 'z', 'Z' },
	{ 'x', 'X' },
	{ 'c', 'C' },
	{ 'v', 'V' },                   /* 0x2F */
	{ 'b', 'B' },
	{ 'n', 'N' },
	{ 'm', 'M' },
	{ ',', '<'},
	{ '.', '>' },
	{ '/', '?' },
	{ 0, 0 },                       /* RSHIFT */
	{ '*', '*' },
	{ 0, 0 },                       /* LALT */
	{ ' ', ' ' },                   /* Space */
	{ 0, 0 },                       /* Capslock */
	{ KEY_F(1), KEY_F(1) },
	{ KEY_F(2), KEY_F(2) },
	{ KEY_F(3), KEY_F(3) },
	{ KEY_F(4), KEY_F(4) },
	{ KEY_F(5), KEY_F(5) },         /* 0x3F */
	{ KEY_F(6), KEY_F(6) },
	{ KEY_F(7), KEY_F(7) },
	{ KEY_F(8), KEY_F(8) },
	{ KEY_F(9), KEY_F(9) },
	{ KEY_F(10), KEY_F(10) },
	{ 0, 0 },                      /* Numlock */
	{ 0, 0 },                      /* Scroll lock */
	{ KEY_HOME, KEY_HOME },
	{ KEY_UP,   KEY_UP },
	{ KEY_PPAGE, KEY_PPAGE },
	{ '-',      '-' },
	{ KEY_LEFT, KEY_LEFT },
	{ 0,        0 },
	{ KEY_RIGHT, KEY_RIGHT },
	{ '-',       '-' },
	{ KEY_END,  KEY_END },         /* 0x4F */
	{ KEY_DOWN, KEY_DOWN },
	{ KEY_NPAGE, KEY_NPAGE },
	{ KEY_IC,    KEY_IC },
	{ KEY_DC,    KEY_DC },
	{ 0, 0 },                     /* sysreq */
	{ 0, 0 },
	{ KEY_F(11), KEY_F(11) },
	{ KEY_F(12), KEY_F(12) },
};

static int cook_scancodes(unsigned char code)
{
	static int modifiers = 0;
	int ch = 0, sc, shift;

	switch (code) {
	case DOWN(SCANCODE_RSHIFT):
	case DOWN(SCANCODE_LSHIFT):
		modifiers |= SHIFT_MODIFIER;
		return 0;
	case UP(SCANCODE_RSHIFT):
	case UP(SCANCODE_LSHIFT):
		modifiers &= ~SHIFT_MODIFIER;
		return 0;
	case UP(SCANCODE_CAPSLOCK):
		if (modifiers & CAPSLOCK_MODIFIER)
			modifiers &= ~CAPSLOCK_MODIFIER;
		else
			modifiers |= CAPSLOCK_MODIFIER;
		return 0;
	case DOWN(SCANCODE_LALT):
		modifiers |= ALT_MODIFIER;
		return 0;
	case UP(SCANCODE_LALT):
		modifiers &= ~ALT_MODIFIER;
		return 0;
	case DOWN(SCANCODE_LCTRL):
		modifiers |= CTRL_MODIFIER;
		return 0;
	case UP(SCANCODE_LCTRL):
		modifiers &= ~CTRL_MODIFIER;
		return 0;
	}

	/* Only process keys on an upstroke. */
	if (!ISUP(code))
		return 0;

	sc = SCANCODE(code);

	if (sc == 0 || sc > 0x59)
		return ERR;

	shift = (modifiers & SHIFT_MODIFIER) ^ (modifiers & CAPSLOCK_MODIFIER);

	ch = shift ? scancode_map[sc].shift : scancode_map[sc].normal;

	if (modifiers & CTRL_MODIFIER)
		ch = (ch >= 0x3F && ch <= 0x5F) ? CTRL(ch) : 0;

	return ch;
}

static int curses_getchar(int delay)
{
	unsigned char c = 0;
	int ret;

	do {
		if (curses_flags & F_ENABLE_CONSOLE)
			c = inb(0x64);

		if ((c & 1) == 0) {

			if ((curses_flags & F_ENABLE_SERIAL) &&
			    serial_havechar()) {
				c = serial_getchar();
				return cook_serial(c);
			}

			if (delay == 0)
				break;

			if (delay > 0) {
				mdelay(100);
				delay--;
			}

			continue;
		}

		c = inb(0x60);

		ret = cook_scancodes(c);

		if (ret != 0) {
			return ret;
		}

	} while (1);

	return ERR;
}

/* === Public functions === */

int wgetch(WINDOW *win)
{
	int delay = -1;

	if (_halfdelay || win->_delay)
		delay = win->_delay ? 0 : _halfdelay;

	return curses_getchar(delay);
}

int nodelay(WINDOW *win, NCURSES_BOOL flag)
{
	win->_delay = flag ? 1 : 0;
	return 0;
}

int halfdelay(int tenths)
{
	if (tenths > 255)
		return ERR;

	_halfdelay = tenths;
	return 0;
}

int nocbreak(void)
{
	/* Remove half delay timeout. */
	_halfdelay = 0;
	return 0;
}

#ifdef CONFIG_VGA_CONSOLE
void curses_enable_vga(int state)
{
	if (state)
		curses_flags |= F_ENABLE_CONSOLE;
	else
		curses_flags &= ~F_ENABLE_CONSOLE;
}
#else
void curses_enable_vga(int state) { }
#endif

#ifdef CONFIG_SERIAL_CONSOLE
void curses_enable_serial(int state)
{
	if (state)
		curses_flags |= F_ENABLE_SERIAL;
	else
		curses_flags &= ~F_ENABLE_SERIAL;
}
#else
void curses_enable_serial(int state) { }
#endif

