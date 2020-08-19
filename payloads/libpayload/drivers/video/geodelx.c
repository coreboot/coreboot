/*
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
#include <pci.h>
#include <video_console.h>
#include <arch/msr.h>
#include "font.h"

/* This is the video mode that we're going to use for our VGA screen */

static const struct mode {
	unsigned int pll;
	unsigned int hactive, hblankstart, hsyncstart;
	unsigned int hsyncend, hblankend, htotal;
	unsigned int vactive, vblankstart, vsyncstart;
	unsigned int vsyncend, vblankend, vtotal;
	unsigned int synccfg;
} vga_mode = {
	.pll = 0x215D,
	.hactive = 640,
	.vactive = 400,
	.hblankstart = 640,
	.hsyncstart =  640 + 40,
	.hsyncend =    640 + 40 + 96,
	.hblankend =   640 + 40 + 96 + 24,
	.htotal =      640 + 40 + 96 + 24,
	.vblankstart = 400,
	.vsyncstart = 400 + 39,
	.vsyncend =   400 + 39 + 2,
	.vblankend =  400 + 39 + 2 + 9,
	.vtotal =     400 + 39 + 2 + 9,
	.synccfg = 0x300,
};

/* These are the color definitions for the 16 standard colors */

static const unsigned int vga_colors[] = {
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

/* Addresses for the various components */

static unsigned long dcaddr;
static unsigned long vgaddr;
static unsigned long gpaddr;
static unsigned long fbaddr;

#define DC (phys_to_virt(dcaddr))
#define VG (phys_to_virt(vgaddr))
#define GP (phys_to_virt(gpaddr))
#define FB ((unsigned char *) phys_to_virt(fbaddr))

static void init_video_mode(void)
{
	unsigned int lo, hi, val;
	int i;

	/* Set the PLL */

	rdmsr(0x4c000015, lo, hi);

	hi = vga_mode.pll;

	lo &= ~0x1008000;
	lo |= 0x01;

	wrmsr(0x4c000015, lo, hi);
	udelay(100);

	for(i = 0; i < 1000; i++) {
		rdmsr(0x4c000015, lo, hi);
		if (lo & 0x2000000)
			break;
	}

	lo &= ~0x01;
	wrmsr(0x4c000015, lo, hi);

	rdmsr(0x48002001, lo, hi);
	lo &= ~0x38;
	wrmsr(0x48002001, lo, hi);

	writel(0x4758, DC + 0x00);

	val = readl(DC + 0x00);

	writel(0, DC + 0x10);
	writel(0, DC + 0x14);
	writel(0, DC + 0x18);

	/* Set up the default scaling */

	val = readl(DC + 0xD4);

	writel((0x4000 << 16) | 0x4000, DC + 0x90);
	writel(0, DC + 0x94);
	writel(val & ~0xf3040000, DC + 0xD4);

	/* Set up the compression (or lack thereof) */
	writel(vga_mode.hactive * vga_mode.vactive | 0x01, DC + 0x2C);

	val = readl(DC + 0x88);
	writel(val & ~0xC00, DC + 0x88);
	writel(0, DC + 0x8C);

	/* Set the pitch */
	writel(vga_mode.hactive >> 3, DC + 0x34);
	writel((vga_mode.hactive + 7) >> 3, DC + 0x30);

	/* Set up default watermarks */

	lo = 0xC0;
	wrmsr(0x80000011, lo, hi);

	/* Write the timings */

	writel((vga_mode.hactive - 1) | ((vga_mode.htotal - 1) << 16),
	       DC + 0x40);

	writel((vga_mode.hblankstart - 1) | ((vga_mode.hblankend - 1) << 16),
	       DC + 0x44);

	writel((vga_mode.hsyncstart - 1) | ((vga_mode.hsyncend - 1) << 16),
	       DC + 0x48);

	writel((vga_mode.vactive - 1) | ((vga_mode.vtotal - 1) << 16),
	       DC + 0x50);

	writel((vga_mode.vblankstart - 1) | ((vga_mode.vblankend - 1) << 16),
	       DC + 0x54);

	writel((vga_mode.vsyncstart - 1) | ((vga_mode.vsyncend - 1) << 16),
	       DC + 0x58);

	writel(((vga_mode.hactive - 1) << 16) | (vga_mode.vactive - 1),
	       DC + 0x5C);

	/* Write the VG configuration */

	writel(0x290000F | vga_mode.synccfg, VG + 0x08);

	/* Turn on the dacs */

	val = readl(VG + 0x50);
	writel((val & ~0xC00) | 0x01, VG + 0x50);

	/* Set the framebuffer base */
	writel(fbaddr, DC + 0x84);

	/* Write the final configuration */

	writel(0xB000059, DC + 0x08);
	writel(0, DC + 0x0C);
	writel(0x2B601, DC + 0x04);
}

static void geodelx_set_palette(int entry, unsigned int color)
{
	writel(entry, DC + 0x70);
	writel(color, DC + 0x74);
}

static void geodelx_scroll_up(void)
{
	unsigned char *dst = FB;
	unsigned char *src = FB + font_height * vga_mode.hactive;
	int y;

	for(y = 0; y < vga_mode.vactive - font_height; y++) {
		memcpy(dst, src, vga_mode.hactive);

		dst += vga_mode.hactive;
		src += vga_mode.hactive;
	}

	for(; y < vga_mode.vactive; y++) {
		memset(dst, 0, vga_mode.hactive);
		dst += vga_mode.hactive;
	}
}

static void geodelx_clear(void)
{
	int row;
	unsigned char *ptr = FB;

	for(row = 0; row < vga_mode.vactive; row++) {
		memset(ptr, 0, vga_mode.hactive);
		ptr += vga_mode.hactive;
	}
}

static void geodelx_putc(u8 row, u8 col, unsigned int ch)
{
	unsigned char *dst;

	unsigned char bg = (ch >> 12) & 0xF;
	unsigned char fg = (ch >> 8) & 0xF;

	int x, y;

	dst = FB + ((row * font_height) * vga_mode.hactive);
	dst += (col * font_width);

	for(y = 0; y < font_height; y++) {

		for(x = font_width - 1; x >= 0; x--)
			dst[font_width - x] = font_glyph_filled(ch, x, y) ?
				fg : bg;

		dst += vga_mode.hactive;
	}
}

static int geodelx_init(void)
{
	pcidev_t dev;
	int i;

	if (!pci_find_device(0x1022, 0x2081, &dev))
		return -1;

	fbaddr = pci_read_resource(dev, 0);
	gpaddr = pci_read_resource(dev, 1);
	dcaddr = pci_read_resource(dev, 2);
	vgaddr = pci_read_resource(dev, 3);

	font_init(vga_mode.hactive);

	init_video_mode();

	/* Set up the palette */

	for(i = 0; i < ARRAY_SIZE(vga_colors); i++) {
		geodelx_set_palette(i, vga_colors[i]);
	}

	geodelx_clear();

	return 0;
}

struct video_console geodelx_video_console = {
	.init = geodelx_init,
	.putc = geodelx_putc,
	.clear = geodelx_clear,
	.scroll_up = geodelx_scroll_up,

	/* TODO .get_cursor */
	/* TODO .set_cursor */
	/* TODO .enable_cursor */

	.columns = 80,
	.rows    = 25
};
