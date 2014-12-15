/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef EDID_H
#define EDID_H

/* structure for communicating EDID information from a raw EDID block to
 * higher level functions.
 * The size of the data types is not critical, so we leave them as
 * unsigned int. We can move more into into this struct as needed.
 */

struct edid {
	char manuf_name[4];
	unsigned int model;
	unsigned int serial;
	unsigned int year;
	unsigned int week;
	unsigned int version[2];
	unsigned int nonconformant;
	unsigned int type;
	/* These next three things used to all be called bpp.
	 * Merriment ensued. The identifier
	 * 'bpp' is herewith banished from our
	 * Kingdom.
	 */
	/* How many bits in the framebuffer per pixel.
	 * Under all reasonable circumstances, it's 32.
	 */
	unsigned int framebuffer_bits_per_pixel;
	/* On the panel, how many bits per color?
	 * In almost all cases, it's 6 or 8.
	 * The standard allows for much more!
	 */
	unsigned int panel_bits_per_color;
	/* On the panel, how many bits per pixel.
	 * On Planet Earth, there are three colors
	 * per pixel, but this is convenient to have here
	 * instead of having 3*panel_bits_per_color
	 * all over the place.
	 */
	unsigned int panel_bits_per_pixel;
	unsigned int xres;
	unsigned int yres;
	unsigned int voltage;
	unsigned int sync;
	unsigned int xsize_cm;
	unsigned int ysize_cm;
	/* used to compute timing for graphics chips. */
	unsigned char phsync;
	unsigned char pvsync;
	unsigned int x_mm;
	unsigned int y_mm;
	unsigned int pixel_clock;
	unsigned int link_clock;
	unsigned int ha;
	unsigned int hbl;
	unsigned int hso;
	unsigned int hspw;
	unsigned int hborder;
	unsigned int va;
	unsigned int vbl;
	unsigned int vso;
	unsigned int vspw;
	unsigned int vborder;
	/* 3 variables needed for coreboot framebuffer.
	 * In most cases, they are the same as the ha
	 * and va variables, but not always, as in the
	 * case of a 1366 wide display.
	 */
	u32 x_resolution;
	u32 y_resolution;
	u32 bytes_per_line;
	/* it is unlikely we need these things. */
	/* if one of these is non-zero, use that one. */
	/* they're aspect * 10 to provide some additional resolution */
	unsigned int aspect_landscape;
	unsigned int aspect_portrait;
	const char *range_class;
	const char *syncmethod;
	const char *stereo;
};

/* Defined in src/lib/edid.c */
int decode_edid(unsigned char *edid, int size, struct edid *out);
void set_vbe_mode_info_valid(struct edid *edid, uintptr_t fb_addr);

#endif /* EDID_H */
