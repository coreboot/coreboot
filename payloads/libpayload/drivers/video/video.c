/*
 * This file is part of the libpayload project.
 *
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

#include <libpayload-config.h>
#include <libpayload.h>
#include <video_console.h>

#if IS_ENABLED(CONFIG_LP_GEODELX_VIDEO_CONSOLE)
extern struct video_console geodelx_video_console;
#endif

#if IS_ENABLED(CONFIG_LP_COREBOOT_VIDEO_CONSOLE)
extern struct video_console coreboot_video_console;
#endif

#if IS_ENABLED(CONFIG_LP_VGA_VIDEO_CONSOLE)
extern struct video_console vga_video_console;
#endif

static struct video_console *console_list[] =
{
#if IS_ENABLED(CONFIG_LP_GEODELX_VIDEO_CONSOLE)
	&geodelx_video_console,
#endif
#if IS_ENABLED(CONFIG_LP_COREBOOT_VIDEO_CONSOLE)
	&coreboot_video_console,
#endif
#if IS_ENABLED(CONFIG_LP_VGA_VIDEO_CONSOLE)
	&vga_video_console,
#endif
};

static struct video_console *console;

static int cursorx;
static int cursory;

void video_get_rows_cols(unsigned int *rows, unsigned int *cols)
{
	if (console) {
		*cols = console->columns;
		*rows = console->rows;
	} else {
		*cols = *rows = 0;
	}
}

static void video_console_fixup_cursor(void)
{
	if (!console)
		return;

	if (cursorx < 0)
		cursorx = 0;

	if (cursory < 0)
		cursory = 0;

	if (cursorx >= console->columns) {
		cursorx = 0;
		cursory++;
	}

	while(cursory >= console->rows) {
		console->scroll_up();
		cursory--;
	}

	if (console->set_cursor)
		console->set_cursor(cursorx, cursory);
}

void video_console_cursor_enable(int state)
{
	if (console && console->enable_cursor)
		console->enable_cursor(state);
}

void video_console_clear(void)
{
	if (console)
		console->clear();

	cursorx = 0;
	cursory = 0;

	if (console && console->set_cursor)
		console->set_cursor(cursorx, cursory);
}

void video_console_putc(u8 row, u8 col, unsigned int ch)
{
	if (console)
		console->putc(row, col, ch);
}

void video_console_putchar(unsigned int ch)
{
	if (!console)
		return;

	/* replace black-on-black with light-gray-on-black.
	 * do it here, instead of in libc/console.c
	 */
	if ((ch & 0xFF00) == 0) {
		ch |= 0x0700;
	}

	switch(ch & 0xFF) {
	case '\r':
		cursorx = 0;
		break;

	case '\n':
		cursorx = 0;
		cursory++;
		break;

	case '\b':
		cursorx--;
		if (cursorx < 0) {
			cursory--;
			cursorx = console->columns;
		}
		break;

	case '\t':
		while(cursorx % 8 && cursorx < console->columns)
			console->putc(cursory, cursorx++, (ch & 0xFF00) | ' ');
		break;
	default:
		console->putc(cursory, cursorx++, ch);
		break;
	}

	video_console_fixup_cursor();
}

void video_printf(int foreground, int background, enum video_printf_align align,
		  const char *fmt, ...)
{
	int i = 0, len;
	char str[200];

	va_list ap;
	va_start(ap, fmt);
	len = vsnprintf(str, ARRAY_SIZE(str), fmt, ap);
	va_end(ap);
	if (len <= 0)
		return;

	/* vsnprintf can return len larger than size. when it happens,
	 * only size-1 characters have been actually written. */
	if (len >= ARRAY_SIZE(str))
		len = ARRAY_SIZE(str) - 1;

	if (len > console->columns) {
		cursorx = 0;
	} else {
		switch (align) {
		case VIDEO_PRINTF_ALIGN_LEFT:
			cursorx = 0;
			break;
		case VIDEO_PRINTF_ALIGN_CENTER:
			cursorx = (console->columns - len) / 2;
			break;
		case VIDEO_PRINTF_ALIGN_RIGHT:
			cursorx = console->columns - len;
			break;
		default:
			break;
		}
	}

	foreground &= 0xf;
	foreground <<= 8;
	background &= 0xf;
	background <<= 12;

	while (str[i])
		video_console_putchar(str[i++] | foreground | background);
}

void video_console_get_cursor(unsigned int *x, unsigned int *y, unsigned int *en)
{
	*x=0;
	*y=0;
	*en=0;

	if (console && console->get_cursor)
		console->get_cursor(x, y, en);

	*x = cursorx;
	*y = cursory;
}

void video_console_set_cursor(unsigned int x, unsigned int y)
{
	cursorx = x;
	cursory = y;
	video_console_fixup_cursor();
}

static struct console_output_driver cons = {
	.putchar = video_console_putchar
};

int video_init(void)
{
	int i;
	unsigned int dummy_cursor_enabled;

	for (i = 0; i < ARRAY_SIZE(console_list); i++) {
		if (console_list[i]->init())
			continue;

		console = console_list[i];

		if (console->get_cursor)
			console->get_cursor((unsigned int*)&cursorx,
					    (unsigned int*)&cursory,
					    &dummy_cursor_enabled);

		if (cursorx) {
			cursorx = 0;
			cursory++;
		}

		video_console_fixup_cursor();
		return 0;
	}
	return 1;
}

int video_console_init(void)
{
	int ret = video_init();
	if (ret)
		return ret;
	console_add_output_driver(&cons);
	return 0;
}
