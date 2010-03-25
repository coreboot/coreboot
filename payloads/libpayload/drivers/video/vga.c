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

#include <libpayload.h>
#include <video_console.h>

struct video_console vga_video_console;

#define VGA_COLOR_WHITE 7

#define CRTC_INDEX      0x3d4
#define CRTC_DATA       0x3d5

#define VIDEO(_r, _c)\
  ((u16 *) (phys_to_virt(0xB8000) + ((_r) * (vga_video_console.columns * 2)) + ((_c) * 2)))

static u8 crtc_read(u8 index)
{
        outb(index, CRTC_INDEX);
        return inb(CRTC_DATA);
}

static void crtc_write(u8 data, u8 index)
{
        outb(index, CRTC_INDEX);
        outb(data, CRTC_DATA);
}

static void vga_get_cursor(unsigned int *x, unsigned int *y, unsigned int *en)
{
	unsigned int addr;
	addr = ((unsigned int) crtc_read(0x0E)) << 8;
	addr += crtc_read(0x0F);

	*x = addr % vga_video_console.columns;
	*y = addr / vga_video_console.columns;

	*en = !(crtc_read(0x0A) & (1 << 5));
}

static void vga_set_cursor(unsigned int x, unsigned int y)
{
	unsigned int addr;

	addr = x + (vga_video_console.columns * y);
	crtc_write(addr >> 8, 0x0E);
	crtc_write(addr, 0x0F);
}

static void vga_enable_cursor(int state)
{
	unsigned char tmp = crtc_read(0x0a);

	if (state == 0)
		tmp |= (1 << 5);

	else
		tmp &= ~(1 << 5);

	crtc_write(tmp, 0x0a);
}

static void vga_clear_line(u8 row, u8 ch, u8 attr)
{
	int col;
	u16 *ptr = VIDEO(row, 0);

	for(col = 0; col < vga_video_console.columns; col++)
		ptr[col] = ((attr & 0xFF) << 8) | (ch & 0xFF);
}

static void vga_scroll_up(void)
{
	u16 *src = VIDEO(1,0);
	u16 *dst = VIDEO(0,0);
	int i;

	for(i = 0; i < (vga_video_console.rows - 1) * vga_video_console.columns; i++)
		*dst++ = *src++;

	vga_clear_line(vga_video_console.rows - 1, ' ', VGA_COLOR_WHITE);
}

static void vga_fill(u8 ch, u8 attr)
{
	u8 row;
	for(row = 0; row < vga_video_console.rows; row++)
		vga_clear_line(row, ch, attr);
}

static void vga_clear(void)
{
	vga_fill(' ', VGA_COLOR_WHITE);
}

static void vga_putc(u8 row, u8 col, unsigned int c)
{
	u16 *ptr = VIDEO(row, col);
	*ptr = (u16) (c & 0xFFFF);
}

static void vga_init_cursor(void)
{
	u8 val;

#define CURSOR_MSL   0x09   /* cursor maximum scan line */
#define CURSOR_START 0x0A   /* cursor start */
#define CURSOR_END   0x0B   /* cursor end */

	val = crtc_read(CURSOR_MSL) & 0x1f;
	crtc_write(0, CURSOR_START);
	crtc_write(val - 2, CURSOR_END);
}

static int vga_init(void)
{
	vga_init_cursor();
	return 0;
}

struct video_console vga_video_console = {
	.init = vga_init,
	.putc = vga_putc,
	.clear = vga_clear,
	.scroll_up = vga_scroll_up,

	.get_cursor = vga_get_cursor,
	.set_cursor = vga_set_cursor,
	.enable_cursor = vga_enable_cursor,

	.columns = 80,
	.rows    = 25
};
