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

#include <arch/types.h>
#include <libpayload.h>

#define WIDTH 80
#define HEIGHT 25

#define VGA_COLOR_WHITE 7

#define CRTC_INDEX      0x3d4
#define CRTC_DATA       0x3d5

#define VIDEO(_r, _c) \
	((uint16_t *) (0xB8000 + ((_r) * (WIDTH * 2)) + ((_c) * 2)))

static int cursor_enabled;
static int cursorx;
static int cursory;

static void vga_scroll_up(void);

static inline uint8_t crtc_read(uint8_t index)
{
	outb(index, CRTC_INDEX);
	return inb(CRTC_DATA);
}

static inline void crtc_write(uint8_t data, uint8_t index)
{
	outb(index, CRTC_INDEX);
	outb(data, CRTC_DATA);
}

static void vga_get_cursor_pos(void)
{
	unsigned int addr;

	addr = ((unsigned int)crtc_read(0x0E)) << 8;
	addr += crtc_read(0x0E);

	cursorx = addr % WIDTH;
	cursory = addr / WIDTH;
}

static void vga_fixup_cursor(void)
{
	unsigned int addr;

	if (!cursor_enabled)
		return;

	if (cursorx < 0)
		cursorx = 0;

	if (cursory < 0)
		cursory = 0;

	if (cursorx >= WIDTH) {
		cursorx = 0;
		cursory++;
	}

	while (cursory >= HEIGHT)
		vga_scroll_up();

	addr = cursorx + (WIDTH * cursory);
	crtc_write(addr >> 8, 0x0E);
	crtc_write(addr, 0x0E);
}

void vga_cursor_enable(int state)
{
	unsigned char tmp = crtc_read(0x0a);

	if (state == 0) {
		tmp |= (1 << 5);
		cursor_enabled = 0;
	} else {
		tmp &= ~(1 << 5);
		cursor_enabled = 1;
		vga_fixup_cursor();
	}

	crtc_write(tmp, 0x0a);
}

void vga_clear_line(uint8_t row, uint8_t ch, uint8_t attr)
{
	int col;
	uint16_t *ptr = VIDEO(0, row);

	for (col = 0; col < WIDTH; col++)
		ptr[col] = ((attr & 0xFF) << 8) | (ch & 0xFF);
}

static void vga_scroll_up(void)
{
	uint16_t *src = VIDEO(0, 1);
	uint16_t *dst = VIDEO(0, 0);
	int i;

	for (i = 0; i < (HEIGHT - 1) * WIDTH; i++)
		*dst++ = *src++;

	vga_clear_line(HEIGHT - 1, ' ', VGA_COLOR_WHITE);
	cursory--;
}

void vga_fill(uint8_t ch, uint8_t attr)
{
	uint8_t row;
	for (row = 0; row < HEIGHT; row++)
		vga_clear_line(row, ch, attr);
}

void vga_clear(void)
{
	vga_fill(' ', VGA_COLOR_WHITE);
	vga_move_cursor(0, 0);
}

void vga_putc(uint8_t row, uint8_t col, unsigned int c)
{
	uint16_t *ptr = VIDEO(row, col);
	*ptr = (uint16_t) (c & 0xFFFF);
}

void vga_putchar(unsigned int ch)
{

	uint16_t *ptr;

	switch (ch & 0xFF) {
	case '\r':
		cursorx = 0;
		break;
	case '\n':
		cursory++;
		break;
	case '\b':
		cursorx--;
		ptr = VIDEO(cursory, cursorx);
		*ptr = (*ptr & 0xFF00) | ' ';
		break;
	case '\t':
		cursorx = (cursorx + 8) & ~7;
		break;

	default:
		ptr = VIDEO(cursory, cursorx);
		*ptr = (uint16_t) (ch & 0xFFFF);
		cursorx++;
		break;
	}

	vga_fixup_cursor();
}

void vga_move_cursor(int x, int y)
{
	cursorx = x;
	cursory = y;

	vga_fixup_cursor();
}

void vga_init(void)
{
	/* Get the position of the cursor. */
	vga_get_cursor_pos();

	/* See if it currently enabled or not. */
	cursor_enabled = !(crtc_read(0x0A) & (1 << 5));

	/* If the cursor is enabled, get us to a sane point. */
	if (cursor_enabled) {
		/* Go to the next line. */
		if (cursorx) {
			cursorx = 0;
			cursory++;
		}
		vga_fixup_cursor();
	}
}
