/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <pc80/vga.h>
#include <pc80/vga_io.h>

#include <string.h>
#include "vga.h"

/*
 * pci io enable should've happened before
 */
void
vga_io_init(void)
{
	vga_enable_mask(0x01, 0x01);

	/* cr io is at 0x3D4/0x3D5 */
	vga_misc_mask(0x01, 0x01);

	/* clear cr0-7 protection */
	vga_cr_mask(0x11, 0x00, 0x80);
}

/*
 *
 */
static void
vga_fb_init(void)
{
	vga_sr_write(0x02, 0x03);
	vga_sr_write(0x03, 0x00);
	vga_sr_write(0x04, 0x02); /* access all 256kB */

	vga_gr_write(0x00, 0x00);
	vga_gr_write(0x01, 0x00);
	vga_gr_write(0x02, 0x00);
	vga_gr_write(0x03, 0x00);
	vga_gr_write(0x04, 0x00);
	vga_gr_write(0x05, 0x10);
	vga_gr_write(0x06, 0x0E); /* map at 0xB8000 */
	vga_gr_write(0x07, 0x00);
	vga_gr_write(0x08, 0xFF);

	/* o/e enable: RAM enable */
	vga_misc_mask(0x22, 0x22);
}

/*
 *
 */
static void
vga_fb_clear(void)
{
	memset((void *)VGA_FB, 0x00, 0x8000);
}

/*
 *
 */
static void
vga_palette_init(void)
{
	size_t i;

	/* set up attribute registers */
	for (i = 0; i < 0x10; i++)
		vga_ar_write(i, i);

	vga_ar_write(0x10, 0x0c);
	vga_ar_write(0x11, 0x00);
	vga_ar_write(0x12, 0x0F);
	vga_ar_write(0x13, 0x08);
	vga_ar_write(0x14, 0x00);

	vga_palette_disable();

	/* load actual palette */
	vga_dac_mask_write(0xFF);

	for (i = 0; i < 0x100; i++) {
		vga_dac_write_address(i);
		vga_dac_data_write(default_vga_palette[i].red);
		vga_dac_data_write(default_vga_palette[i].green);
		vga_dac_data_write(default_vga_palette[i].blue);
	}
}

/*
 *
 */
static void
vga_mode_set(int hdisplay, int hblankstart, int hsyncstart, int hsyncend,
	     int hblankend, int htotal, int vdisplay, int vblankstart,
	     int vsyncstart, int vsyncend, int vblankend, int vtotal,
	     int stride)
{
	/* htotal: 2080 */
	htotal /= 8;
	htotal -= 5;
	vga_cr_write(0x00, htotal);

	/* hdisplay: 2048 */
	hdisplay /= 8;
	hdisplay -= 1;
	vga_cr_write(0x01, hdisplay);

	/* hblankstart: 2048 */
	hblankstart /= 8;
	hblankstart -= 1;
	vga_cr_write(0x02, hblankstart);

	/* hblankend: hblankstart + 512 */
	hblankend /= 8;
	hblankend -= 1;
	vga_cr_mask(0x03, hblankend, 0x1F);
	vga_cr_mask(0x05, hblankend << 2, 0x80);

	/* hsyncstart: 255 * 8: 2040 */
	vga_cr_write(0x04, hsyncstart / 8);

	/* hsyncend: hsyncstart + 255 */
	vga_cr_mask(0x05, hsyncend / 8, 0x1F);

	/* vtotal: 1025 */
	vtotal -= 2;
	vga_cr_write(0x06, vtotal);
	vga_cr_mask(0x07, vtotal >> 8, 0x01);
	vga_cr_mask(0x07, vtotal >> 4, 0x20);

	/* vdisplay: 1024 */
	vdisplay -= 1;
	vga_cr_write(0x12, vdisplay);
	vga_cr_mask(0x07, vdisplay >> 7, 0x02);
	vga_cr_mask(0x07, vdisplay >> 3, 0x40);

	/* vblankstart: 1024 */
	vblankstart -= 1;
	vga_cr_write(0x15, vblankstart);
	vga_cr_mask(0x07, vblankstart >> 5, 0x08);
	vga_cr_mask(0x09, vblankstart >> 4, 0x20);

	/* vblankend: vblankstart + 256 */
	vblankend -= 1;
	vga_cr_write(0x16, vblankend);

	/* vsyncstart: 1023 */
	vga_cr_write(0x10, vsyncstart);
	vga_cr_mask(0x07, vsyncstart >> 6, 0x04);
	vga_cr_mask(0x07, vsyncstart >> 2, 0x80);

	/* vsyncend: vsyncstart + 16 */
	vga_cr_mask(0x11, vsyncend, 0x0F);

	/* stride */
	vga_cr_write(0x13, stride / 8);

	/* line compare */
	vga_cr_write(0x18, 0xFF);
	vga_cr_mask(0x07, 0x10, 0x10);
	vga_cr_mask(0x09, 0x40, 0x40);

	vga_misc_mask(0x44, 0xCC); /* set up clock: 27mhz and h/vsync */

	vga_cr_mask(0x09, 0x00, 0x80); /* disable doublescan */
}

static void
vga_font_8x16_load(void)
{
	unsigned char *p;
	size_t i, j;
	unsigned char sr2, sr4, gr5, gr6;

#define height 16
#define count 256

	sr2 = vga_sr_read(0x02);
	sr4 = vga_sr_read(0x04);
	gr5 = vga_gr_read(0x05);
	gr6 = vga_gr_read(0x06);

	/* disable odd/even */
	vga_sr_mask(0x04, 0x04, 0x04);
	vga_gr_mask(0x05, 0x00, 0x10);
	vga_gr_mask(0x06, 0x00, 0x02);

	/* plane 2 */
	vga_sr_write(0x02, 0x04);
	p = (unsigned char *)VGA_FB;
	for (i = 0; i < count; i++) {
		for (j = 0; j < 32; j++) {
			if (j < height)
				*p = vga_font_8x16[i][j];
			else
				*p = 0x00;
			p++;
		}
	}

	vga_gr_write(0x06, gr6);
	vga_gr_write(0x05, gr5);
	vga_sr_write(0x04, sr4);
	vga_sr_write(0x02, sr2);

	/* set up font size */
	vga_cr_mask(0x09, 16 - 1, 0x1F);
}

/*
 *
 */
void
vga_cursor_enable(int enable)
{
	if (enable)
		vga_cr_mask(0x0A, 0x00, 0x20);
	else
		vga_cr_mask(0x0A, 0x20, 0x20);
}

/*
 *
 */
void
vga_cursor_reset(void)
{
	vga_cr_write(0x0A, 0x2E);
	vga_cr_write(0x0B, 0x0E);
	vga_cr_write(0x0E, 0x00);
	vga_cr_write(0x0F, 0x00);
}

/*
 *
 */
void
vga_cursor_set(unsigned int line, unsigned int character)
{
	unsigned int offset = (VGA_COLUMNS * line + character) & 0xFFFF;

	vga_cr_write(0x0A, 0x0E);
	vga_cr_write(0x0B, 0x0E);
	vga_cr_write(0x0E, offset >> 8);
	vga_cr_write(0x0F, offset & 0xFF);
}

/*
 *
 */
void
vga_frame_set(unsigned int line, unsigned int character)
{
	unsigned int offset = (VGA_COLUMNS * line + character) & 0xFFFF;

	vga_cr_write(0x0C, offset >> 8);
	vga_cr_write(0x0D, offset & 0xFF);
}

static void
vga_write_at_offset(unsigned int line, unsigned int offset, const char *string)
{
	if (!string)
		return;

	unsigned short *p = (unsigned short *)VGA_FB + (VGA_COLUMNS * line) + offset;
	size_t i, len = strlen(string);

	for (i = 0; i < (VGA_COLUMNS - offset); i++) {
		if (i < len)
			p[i] = 0x0F00 | string[i];
		else
			p[i] = 0x0F00;
	}
}

/*
 * simply fills a line with the given string.
 */
void
vga_line_write(unsigned int line, const char *string)
{
	vga_write_at_offset(line, 0, string);
}

void
vga_write_text(enum VGA_TEXT_ALIGNMENT alignment, unsigned int line, const char *string)
{
	char str[VGA_COLUMNS * VGA_LINES] = {0};
	memcpy(str, string, strnlen(string, sizeof(str) - 1));

	char *token = strtok(str, "\n");

	while (token != NULL) {
		size_t offset = VGA_COLUMNS - strnlen(token, VGA_COLUMNS);
		switch (alignment) {
		case VGA_TEXT_CENTER:
			vga_write_at_offset(line++, offset/2, token);
			break;
		case VGA_TEXT_RIGHT:
			vga_write_at_offset(line++, offset, token);
			break;
		case VGA_TEXT_LEFT:
		default:
			vga_write_at_offset(line++, 0, token);
			break;
		}
		token = strtok(NULL, "\n");
	}
}

/*
 * set up everything to get a basic 80x25 textmode.
 */
void
vga_textmode_init(void)
{
	vga_sr_write(0x00, 0x01); /* clear reset */
	vga_sr_write(0x01, 0x00);

	/* set up cr */
	vga_cr_mask(0x03, 0x80, 0xE0);
	vga_cr_mask(0x05, 0x00, 0x60);

	vga_cr_write(0x08, 0x00);

	vga_cr_write(0x14, 0x00); /* */

	vga_cr_write(0x17, 0x23);

	vga_palette_init();

	vga_mode_set(640, 648, 680, 776, 792, 800,
		     400, 407, 412, 414, 442, 449, 320);

	vga_cursor_reset();
	vga_frame_set(0, 0);

	vga_fb_init();
	vga_fb_clear();
	vga_font_8x16_load();

	vga_sr_mask(0x00, 0x02, 0x02); /* take us out of reset */
	vga_cr_mask(0x17, 0x80, 0x80); /* sync! */
}
