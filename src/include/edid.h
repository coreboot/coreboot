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
 */

#ifndef EDID_H
#define EDID_H

enum edid_modes {
	EDID_MODE_640x480_60Hz,
	EDID_MODE_720x480_60Hz,
	EDID_MODE_1280x720_60Hz,
	EDID_MODE_1920x1080_60Hz,
	NUM_KNOWN_MODES,

	EDID_MODE_AUTO
};

struct edid_mode {
	const char *name;
	unsigned int pixel_clock;
	int lvds_dual_channel;
	unsigned int refresh;
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
	unsigned char phsync;
	unsigned char pvsync;
	unsigned int x_mm;
	unsigned int y_mm;
};

/* structure for communicating EDID information from a raw EDID block to
 * higher level functions.
 * The size of the data types is not critical, so we leave them as
 * unsigned int. We can move more into into this struct as needed.
 */

struct edid {
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
	/* used to compute timing for graphics chips. */
	struct edid_mode mode;
	u8 mode_is_supported[NUM_KNOWN_MODES];
	unsigned int link_clock;
	/* 3 variables needed for coreboot framebuffer.
	 * In most cases, they are the same as the ha
	 * and va variables, but not always, as in the
	 * case of a 1366 wide display.
	 */
	u32 x_resolution;
	u32 y_resolution;
	u32 bytes_per_line;

	int hdmi_monitor_detected;
};

/* Defined in src/lib/edid.c */
int decode_edid(unsigned char *edid, int size, struct edid *out);
void set_vbe_mode_info_valid(struct edid *edid, uintptr_t fb_addr);
int set_display_mode(struct edid *edid, enum edid_modes mode);

#endif /* EDID_H */
