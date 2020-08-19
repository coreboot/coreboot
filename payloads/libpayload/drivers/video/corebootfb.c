/*
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 coresystems GmbH
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
#include <coreboot_tables.h>
#include <pci.h>
#include <video_console.h>
#include "font.h"

struct video_console coreboot_video_console;

static unsigned int cursor_x = 0, cursor_y = 0, cursor_en = 0;

/* color definitions for the 16 standard colors */

#define VGA_COLOR_DEFAULT 7
static const u32 vga_colors[] = {
	// BLUE      | GREEN       | RED
	(0x00 << 16) | (0x00 << 8) | 0x00,
	(0xAA << 16) | (0x00 << 8) | 0x00,
	(0x00 << 16) | (0xAA << 8) | 0x00,
	(0xAA << 16) | (0x55 << 8) | 0x00,
	(0x00 << 16) | (0x00 << 8) | 0xAA,
	(0xAA << 16) | (0x00 << 8) | 0xAA,
	(0x00 << 16) | (0xAA << 8) | 0xAA,
	(0xAA << 16) | (0xAA << 8) | 0xAA,
	(0x55 << 16) | (0x55 << 8) | 0x55,
	(0xFF << 16) | (0x55 << 8) | 0x55,
	(0x55 << 16) | (0xFF << 8) | 0x55,
	(0xFF << 16) | (0xFF << 8) | 0x55,
	(0x55 << 16) | (0x55 << 8) | 0xFF,
	(0xFF << 16) | (0x55 << 8) | 0xFF,
	(0x55 << 16) | (0xFF << 8) | 0xFF,
	(0xFF << 16) | (0xFF << 8) | 0xFF,
};

static struct cb_framebuffer fbinfo;
static unsigned short *chars;

/* Shorthand for up-to-date virtual framebuffer address */
#define FB ((unsigned char *)phys_to_virt(fbinfo.physical_address))

static void corebootfb_scroll_up(void)
{
	unsigned char *dst = FB;
	unsigned char *src = FB + (fbinfo.bytes_per_line * font_height);
	int y;

	/* Scroll all lines up */
	for (y = 0; y < fbinfo.y_resolution - font_height; y++) {
		memcpy(dst, src, fbinfo.x_resolution * (fbinfo.bits_per_pixel >> 3));

		dst += fbinfo.bytes_per_line;
		src += fbinfo.bytes_per_line;
	}

	/* Erase last line */
	dst = FB + (fbinfo.y_resolution - font_height) * fbinfo.bytes_per_line;

	for (; y < fbinfo.y_resolution; y++) {
		memset(dst, 0, fbinfo.x_resolution * (fbinfo.bits_per_pixel >> 3));
		dst += fbinfo.bytes_per_line;
	}

	/* And update the char buffer */
	dst = (unsigned char *)chars;
	src = (unsigned char *)(chars + coreboot_video_console.columns);
	memcpy(dst, src, coreboot_video_console.columns *
			(coreboot_video_console.rows - 1) * 2);
	int column;
	for (column = 0; column < coreboot_video_console.columns; column++)
		chars[(coreboot_video_console.rows - 1) * coreboot_video_console.columns + column] = (VGA_COLOR_DEFAULT << 8);

	cursor_y--;
}

static void corebootfb_clear(void)
{
	int row, column;
	unsigned char *ptr = FB;

	/* Clear the screen */
	for (row = 0; row < fbinfo.y_resolution; row++) {
		memset(ptr, 0, fbinfo.x_resolution * (fbinfo.bits_per_pixel >> 3));
		ptr += fbinfo.bytes_per_line;
	}

	/* And update the char buffer */
	for(row = 0; row < coreboot_video_console.rows; row++)
		for (column = 0; column < coreboot_video_console.columns; column++)
			chars[row * coreboot_video_console.columns + column] = (VGA_COLOR_DEFAULT << 8);
}

static void corebootfb_putchar(u8 row, u8 col, unsigned int ch)
{
	unsigned char *dst;

	unsigned char bg = (ch >> 12) & 0xF;
	unsigned char fg = (ch >> 8) & 0xF;
	u32 fgval = 0, bgval = 0;
	u16 *dst16;
	u32 *dst32;

	int x, y;

	if (fbinfo.bits_per_pixel > 8) {
		bgval = ((((vga_colors[bg] >> 0) & 0xff) >> (8 - fbinfo.blue_mask_size)) << fbinfo.blue_mask_pos) |
			((((vga_colors[bg] >> 8) & 0xff) >> (8 - fbinfo.green_mask_size)) << fbinfo.green_mask_pos) |
			((((vga_colors[bg] >> 16) & 0xff) >> (8 - fbinfo.red_mask_size)) << fbinfo.red_mask_pos);
		fgval = ((((vga_colors[fg] >> 0) & 0xff) >> (8 - fbinfo.blue_mask_size)) << fbinfo.blue_mask_pos) |
			((((vga_colors[fg] >> 8) & 0xff) >> (8 - fbinfo.green_mask_size)) << fbinfo.green_mask_pos) |
			((((vga_colors[fg] >> 16) & 0xff) >> (8 - fbinfo.red_mask_size)) << fbinfo.red_mask_pos);
	}

	dst = FB + ((row * font_height) * fbinfo.bytes_per_line);
	dst += (col * font_width * (fbinfo.bits_per_pixel >> 3));

	for(y = 0; y < font_height; y++) {
		for(x = font_width - 1; x >= 0; x--) {

			switch (fbinfo.bits_per_pixel) {
			case 8: /* Indexed */
				dst[(font_width - x) * (fbinfo.bits_per_pixel >> 3)] = font_glyph_filled(ch, x, y) ?  fg : bg;
				break;
			case 16: /* 16 bpp */
				dst16 = (u16 *)(dst + (font_width - x) * (fbinfo.bits_per_pixel >> 3));
				*dst16 = font_glyph_filled(ch, x, y) ? fgval : bgval;
				break;
			case 24: /* 24 bpp */
				if (font_glyph_filled(ch, x, y)) {
					dst[(font_width - x) * (fbinfo.bits_per_pixel >> 3) + 0] = fgval & 0xff;
					dst[(font_width - x) * (fbinfo.bits_per_pixel >> 3) + 1] = (fgval >> 8) & 0xff;
					dst[(font_width - x) * (fbinfo.bits_per_pixel >> 3) + 2] = (fgval >> 16) & 0xff;
				} else {
					dst[(font_width - x) * (fbinfo.bits_per_pixel >> 3) + 0] = bgval & 0xff;
					dst[(font_width - x) * (fbinfo.bits_per_pixel >> 3) + 1] = (bgval >> 8) & 0xff;
					dst[(font_width - x) * (fbinfo.bits_per_pixel >> 3) + 2] = (bgval >> 16) & 0xff;
				}
				break;
			case 32: /* 32 bpp */
				dst32 = (u32 *)(dst + (font_width - x) * (fbinfo.bits_per_pixel >> 3));
				*dst32 = font_glyph_filled(ch, x, y) ? fgval : bgval;
				break;
			}
		}

		dst += fbinfo.bytes_per_line;
	}
}

static void corebootfb_putc(u8 row, u8 col, unsigned int ch)
{
	chars[row * coreboot_video_console.columns + col] = ch;
	corebootfb_putchar(row, col, ch);
}

static void corebootfb_update_cursor(void)
{
	int ch, paint;
	if(cursor_en) {
		ch = chars[cursor_y * coreboot_video_console.columns + cursor_x];
		paint = (ch & 0xff) | ((ch << 4) & 0xf000) | ((ch >> 4) & 0x0f00);
	} else {
		paint = chars[cursor_y * coreboot_video_console.columns + cursor_x];
	}

	if (cursor_y < coreboot_video_console.rows)
		corebootfb_putchar(cursor_y, cursor_x, paint);
}

static void corebootfb_enable_cursor(int state)
{
	cursor_en = state;
	corebootfb_update_cursor();
}

static void corebootfb_get_cursor(unsigned int *x, unsigned int *y, unsigned int *en)
{
	*x = cursor_x;
	*y = cursor_y;
	*en = cursor_en;
}

static void corebootfb_set_cursor(unsigned int x, unsigned int y)
{
	int cursor_remember = cursor_en;
	if (cursor_remember)
		corebootfb_enable_cursor(0);

	cursor_x = x;
	cursor_y = y;

	if (cursor_remember)
		corebootfb_enable_cursor(1);
}

static int corebootfb_init(void)
{
	if (!lib_sysinfo.framebuffer.physical_address)
		return -1;

	fbinfo = lib_sysinfo.framebuffer;

	font_init(fbinfo.x_resolution);

	/* Draw centered on the framebuffer if requested and feasible, */
	const int center =
		IS_ENABLED(CONFIG_LP_COREBOOT_VIDEO_CENTERED)
		&& coreboot_video_console.columns * font_width <= fbinfo.x_resolution
		&& coreboot_video_console.rows * font_height <= fbinfo.y_resolution;
	/* adapt to the framebuffer size, otherwise. */
	if (!center) {
		coreboot_video_console.columns = fbinfo.x_resolution / font_width;
		coreboot_video_console.rows = fbinfo.y_resolution / font_height;
	}

	chars = malloc(coreboot_video_console.rows *
		       coreboot_video_console.columns * 2);
	if (!chars)
		return -1;

	// clear boot splash screen if there is one.
	corebootfb_clear();

	if (center) {
		fbinfo.physical_address +=
			(fbinfo.x_resolution - coreboot_video_console.columns * font_width)
				/ 2 * fbinfo.bits_per_pixel / 8
			+ (fbinfo.y_resolution - coreboot_video_console.rows * font_height)
				/ 2 * fbinfo.bytes_per_line;
		fbinfo.x_resolution = coreboot_video_console.columns * font_width;
		fbinfo.y_resolution = coreboot_video_console.rows * font_height;
	}

	return 0;
}

struct video_console coreboot_video_console = {
	.init = corebootfb_init,
	.putc = corebootfb_putc,
	.clear = corebootfb_clear,
	.scroll_up = corebootfb_scroll_up,

	.get_cursor = corebootfb_get_cursor,
	.set_cursor = corebootfb_set_cursor,
	.enable_cursor = corebootfb_enable_cursor,

	.columns = 80,
	.rows    = 25
};
