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

#include <libpayload-config.h>
#include <usb/usb.h>
#include "local.h"

static int _halfdelay = 0;

/* ============== Serial ==================== */

#ifdef CONFIG_SERIAL_CONSOLE
/* We treat serial like a vt100 terminal.  For now we
   do the cooking in here, but we should probably eventually
   pass it to dedicated vt100 code */

static int getkeyseq(char *buffer, int len, int max)
{
	int i;

	while (1) {
		for(i = 0; i < 75; i++) {
			if (serial_havechar())
				break;
			mdelay(1);
		}

		if (i == 75)
			return len;

		buffer[len++] = serial_getchar();
		if (len == max)
			return len;
	}
}

static struct {
	const char *seq;
	int key;
} escape_codes[] = {
	{ "[A", KEY_UP },
	{ "[B", KEY_DOWN },
	{ "[C", KEY_RIGHT },
	{ "[D", KEY_LEFT },
	{ "[F", KEY_END },
	{ "[H", KEY_HOME },
	{ "[2~", KEY_IC },
	{ "[3~", KEY_DC },
	{ "[5~", KEY_PPAGE },
	{ "[6~", KEY_NPAGE },
	{ "OP", KEY_F(1) },
	{ "OQ", KEY_F(2) },
	{ "OR", KEY_F(3) },
	{ "OS", KEY_F(4) },
	{ "[15~", KEY_F(5) },
	{ "[17~", KEY_F(6) },
	{ "[18~", KEY_F(7) },
	{ "[19~", KEY_F(8) },
	{ "[20~", KEY_F(9) },
	{ "[21~", KEY_F(10) },
	{ "[23~", KEY_F(11) },
	{ "[24~", KEY_F(12) },
	{ NULL },
};

static int handle_escape(void)
{
	char buffer[5];
	int len = getkeyseq(buffer, 0, sizeof(buffer));
	int i, t;

	if (len == 0)
		return 27;

	for(i = 0; escape_codes[i].seq != NULL; i++) {
		const char *p = escape_codes[i].seq;

		for(t = 0; t < len; t++) {
			if (!*p || *p != buffer[t])
				break;
			p++;
		}

		if (t == len)
			return escape_codes[i].key;
	}

	return 0;
}

static int cook_serial(unsigned char ch)
{
	switch(ch) {
	case 8:
		return KEY_BACKSPACE;

	case 13:
		return KEY_ENTER;

	case 27:
		return handle_escape();

	default:
		return ch;
	}
}
#endif

/* ================ Keyboard ================ */

static int curses_getchar(int _delay)
{
#if defined(CONFIG_USB_HID) || defined(CONFIG_PC_KEYBOARD) || defined(CONFIG_SERIAL_CONSOLE)
	unsigned short c;
#endif

	do {
#ifdef CONFIG_USB_HID
		usb_poll();
		if ((curses_flags & F_ENABLE_CONSOLE) &&
		    usbhid_havechar()) {
			c = usbhid_getchar();
			if (c != 0) return c;
		}
#endif
#ifdef CONFIG_PC_KEYBOARD
		if ((curses_flags & F_ENABLE_CONSOLE) &&
		    keyboard_havechar()) {
			c = keyboard_getchar();
			if (c != 0) return c;
		}
#endif

#ifdef CONFIG_SERIAL_CONSOLE
		if ((curses_flags & F_ENABLE_SERIAL) &&
		    serial_havechar()) {
			c = serial_getchar();
			return cook_serial(c);
		}
#endif

		if (_delay == 0) {
			break;
		} else if (_delay >= 10) {
			mdelay(10);
			_delay -= 10;
		} else if (_delay > 0) {
			mdelay(_delay);
			_delay = 0;
		}
	} while (1);

	return ERR;
}

/* === Public functions === */

int wgetch(WINDOW *win)
{
	int _delay = -1;

	if (_halfdelay)
		_delay = _halfdelay;
	else
		_delay = win->_delay;

	return curses_getchar(_delay);
}

int nodelay(WINDOW *win, NCURSES_BOOL flag)
{
	win->_delay = flag ? 0 : -1;
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

#ifdef CONFIG_VGA_VIDEO_CONSOLE
void curses_enable_vga(int state)
{
	if (state)
		curses_flags |= F_ENABLE_CONSOLE;
	else
		curses_flags &= ~F_ENABLE_CONSOLE;
}

int curses_vga_enabled(void)
{
	return (curses_flags & F_ENABLE_CONSOLE) != 0;
}
#else
void curses_enable_vga(int state) { }
int curses_vga_enabled(void) { return 0; }
#endif

#ifdef CONFIG_SERIAL_CONSOLE
void curses_enable_serial(int state)
{
	if (state)
		curses_flags |= F_ENABLE_SERIAL;
	else
		curses_flags &= ~F_ENABLE_SERIAL;
}

int curses_serial_enabled(void)
{
	return (curses_flags & F_ENABLE_SERIAL) != 0;
}

#else
void curses_enable_serial(int state) { }
int curses_serial_enabled(void) { return 0; }
#endif

