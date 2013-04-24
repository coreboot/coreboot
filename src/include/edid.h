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
	unsigned int bpp;
	unsigned int xres;
	unsigned int yres;
	unsigned int voltage;
	unsigned int sync;
	unsigned int xsize_cm;
	unsigned int ysize_cm;
	/* used to compute timing for graphics chips. */
	unsigned char phsync;
	unsigned char pvsync;
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
	/* it is unlikely we need these things. */
	/* if one of these is non-zero, use that one. */
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
