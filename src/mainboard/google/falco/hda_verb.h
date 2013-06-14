/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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

static const u32 mainboard_cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0283,	// Codec Vendor / Device ID: Realtek ALC283
	0x10ec0283,	// Subsystem ID
	0x0000000b,	// Number of jacks (NID entries)

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x10ec0283 */
	0x00172083,
	0x00172102,
	0x001722ec,
	0x00172310,

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x12) DMIC - interior mobile lid */
	0x01271c10,
	0x01271d10,
	0x01271ea6,
	0x01271fb7,

	/* Pin Complex (NID 0x14) SPKR-OUT PORTD */
	0x01471c10, // group 1, front left/right
	0x01471d01, // no connector, no jack detect
	0x01471e17, // speaker out, analog
	0x01471f90, // fixed function, internal

	/* Pin Complex (NID 0x17)  */
	0x01771cf0,
	0x01771d11,
	0x01771e11,
	0x01771f41,

	/* Pin Complex (NID 0x18)  MIC1 PORTB */
	0x01971c11, // group 1, cap 1
	0x01971d10, // black, jack detect
	0x01971ea7, // mic in, analog
	0x01971f03, // connector, left panel

	/* Pin Complex (NID 0x19)  MIC2 PORTF */
	0x01871cf0,
	0x01871d11,
	0x01871e11,
	0x01871f41,

	/* Pin Complex (NID 0x1A)  LINE1 PORTC */
	0x01a71cf0,
	0x01a71d11,
	0x01a71e11,
	0x01a71f41,

	/* Pin Complex (NID 0x1B)  LINE2 PORTE */
	0x01a71cf0,
	0x01a71d11,
	0x01a71e11,
	0x01a71f41,

	/* Pin Complex (NID 0x1d)  PCBeep */
	0x01d71c2d, // eapd low on ex-amp, laptop, custom enable
	0x01d71d81, // mute spkr on hpout
	0x01d71e15, // pcbeep en able, checksum
	0x01d71f40, // no physical, internal

	/* Pin Complex (NID 0x1E)  SPDIF-OUT */
	0x01e71cf0,
	0x01e71d11,
	0x01e71e11,
	0x01e71f41,

	/* Pin Complex (NID 0x21) HPOUT PORT-I */
	0x02171c1f, // group1,
	0x02171d10, // black, jack detect
	0x02171e21, // HPOut, 1/8 stereo
	0x02171f03, // connector, left panel

};

