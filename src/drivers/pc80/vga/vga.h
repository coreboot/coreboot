/*
 * This file is part of the coreboot project.
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

#ifndef _VGA_H
#define _VGA_H

/*
 * Basic palette.
 */
struct palette {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

extern const struct palette default_vga_palette[0x100];

extern const unsigned char vga_font_8x16[256][16];

#endif /* _VGA_H */
