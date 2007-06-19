/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Corey Osgood <corey@slightlyhackish.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef NORTHBRIDGE_INTEL_I82810_RAMINIT_H
#define NORTHBRIDGE_INTEL_I82810_RAMINIT_H

/* The 82810 supports max. 2 dual-sided DIMMs. */
#define DIMM_SOCKETS	2

struct mem_controller {
	device_t d0;
	uint16_t channel0[DIMM_SOCKETS];
};

/* The following table has been bumped over to this header to avoid clutter in 
 * raminit.c. It's used to translate the value read from SPD Byte 31 to a value
 * the northbridge can understand in DRP, aka Rx52[7:4], [3:0]. Where most
 * northbridges have some sort of simple calculation that can be done for this,
 * I haven't yet figured out one for this northbridge. Until someone does,
 * this table is necessary.
 */

/* TODO: Find a better way of doing this. */

static const uint8_t translate_spd_to_i82810[] = {
	/* Note: 4MB sizes are not supported, so dual-sided DIMMs with a 4MB 
	 * side can't be either, at least for now.
	 */
	/* TODO: For above case, only use the other side if > 4MB, and get some
	 * of these DIMMs to test it with. Same for unsupported 128/x sizes.
	 */

		/*   SPD Byte 31	Memory Size [Side 1/2]	*/
	0xff,	/*	0x01		No memory	*/
	0xff,	/*	0x01		 4/0		*/
	0x01,	/*	0x02		 8/0		*/
	0xff,	/*	0x03		 8/4		*/
	0x04,	/*	0x04		16/0 or 16	*/
	0xff,	/*	0x05		16/4		*/
	0x05,	/*	0x06		16/8		*/
	0xff,	/*	0x07		Invalid		*/
	0x07,	/*	0x08		32/0 or 32	*/
	0xff,	/*	0x09		32/4		*/
	0xff,	/*	0x0A		32/8		*/
	0xff,	/*	0x0B		Invalid		*/
	0x08,	/*	0x0C		32/16		*/
	0xff, 0xff, 0xff, /* 0x0D-0F	Invalid		*/
	0x0a,	/*	0x10		64/0 or 64	*/
	0xff,	/*	0x11		64/4		*/
	0xff,	/*	0x12		64/8		*/
	0xff,	/*	0x13		Invalid		*/
	0xff,	/*	0x14		64/16		*/
	0xff, 0xff, 0xff, /* 0x15-17	Invalid		*/
	0x0b,	/*	0x18		64/32		*/
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 0x19-1f Invalid */
	0x0d,	/*	0x20		128/0 or 128	*/
	/* These configurations are not supported by the i810 */
	0xff,	/*	0x21		128/4		*/
	0xff,	/*	0x22		128/8		*/
	0xff,	/*	0x23		Invalid		*/
	0xff,	/*	0x24		128/16		*/
	0xff, 0xff, 0xff, /* 0x25-27	Invalid		*/
	0xff,	/*	0x28		128/32		*/
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 0x29-2f Invalid */
	0x0e,	/*	0x30		128/64		*/
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, /* 0x31-3f	Invalid		*/
	0x0f,	/*	0x40		256/0 or 256	*/
	/* Anything larger is not supported by the 82810. */
};

#endif				/* NORTHBRIDGE_INTEL_I82810_RAMINIT_H */
