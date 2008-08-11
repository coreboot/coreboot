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

#include <config.h>
#include <libpayload.h>
#include <video_console.h>

#ifdef CONFIG_GEODE_VIDEO_CONSOLE
extern struct video_console geode_video_console;
#endif

#ifdef CONFIG_VGA_VIDEO_CONSOLE
extern struct video_console vga_video_console;
#endif

static struct video_console *console_list[] =
{
#ifdef CONFIG_GEODE_VIDEO_CONSOLE
	&geode_video_console,
#endif
#ifdef CONFIG_VGA_VIDEO_CONSOLE
	&vga_video_console,
#endif
};

static struct video_console *console;

static unsigned int cursorx;
static unsigned int cursory;
static unsigned int cursor_enabled = 1;

static void video_console_fixup_cursor(void)
{
	if (!cursor_enabled)
		return;

	if (cursorx < 0)
		cursorx = 0;

	if (cursory < 0)
		cursory = 0;

	if (cursorx > VIDEO_COLS) {
		cursorx = 0;
		cursory++;
	}

	while(cursory >= VIDEO_ROWS) {
		console->scroll_up();
		cursory--;
	}

	if (console && console->set_cursor)
		console->set_cursor(cursorx, cursory);
}

void video_console_cursor_enable(int state)
{
	if (console && console->enable_cursor)
		console->enable_cursor(state);

	cursor_enabled = state;

	if (cursor_enabled)
		video_console_fixup_cursor();
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
	switch(ch & 0xFF) {
	case '\r':
		cursorx = 0;
		break;

	case '\n':
		cursory++;
		break;

	case '\b':
		cursorx--;
		break;

	case '\t':
		while(cursorx % 8 && cursorx < VIDEO_COLS) {
			if (console)
				console->putc(cursory, cursorx, (ch & 0xFF00) | ' ');

			cursorx++;
		}
		break;
	default:
		if (console)
			console->putc(cursory, cursorx++, ch);
		break;
	}

	video_console_fixup_cursor();
}

int video_console_init(void)
{
		int i;

		for(i = 0; i < ARRAY_SIZE(console_list); i++) {
			if (console_list[i]->init())
				continue;

			console = console_list[i];

			if (console->get_cursor)
				console->get_cursor(&cursorx, &cursory, &cursor_enabled);

			if (cursorx) {
				cursorx = 0;
				cursory++;
			}

			video_console_fixup_cursor();
			return 0;
		}

		return 0;
}

