/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Uwe Hermann <uwe@hermann-uwe.de>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * HDA codec soldered onto the ASUS M2N-E:
 * Analog Devices AD1988B (High Definition Audio SoundMAX Codec), rev. 0x100200
 * http://www.analog.com/static/imported-files/data_sheets/AD1988A_1988B.pdf
 */

static const u32 mainboard_cim_verb_data[] = {
	/* coreboot specific header */
	0x11d4198b,	/* Codec Vendor / Device ID: Analog Devices AD1988B */
	0x104381f6,	/* Subsystem ID (0x1043 == ASUS) */
	0x0000000d,	/* Number of "pin complex" entries in the table */

	/* NID 0x01, FUNCTION, Designates this device as an audio codec */
	/* Set the Implementation ID (IID), here: 0x104381f6. */
	0x001720f6,
	0x00172181,
	0x00172243,
	0x00172310,

	/* "Pin complex" HDA widgets. Comments: Node ID, Name, Description. */

	/* NID 0x11, Port A, Front panel headphone jack */
	0x01171c30,
	0x01171d41,
	0x01171e21,
	0x01171f02,

	/* NID 0x12, Port D, Rear panel front speaker jack */
	0x01271c10,
	0x01271d40,
	0x01271e01,
	0x01271f01,

	/* NID 0x13, MONO_OUT, Monaural output pin */
	0x01371cf0,
	0x01371d11,
	0x01371e17,
	0x01371f51,

	/* NID 0x14, Port B, Front panel microphone jack */
	0x01471c22,
	0x01471d91,
	0x01471ea1,
	0x01471f02,

	/* NID 0x15, Port C, Rear panel line-in jack */
	0x01571c21,
	0x01571d30,
	0x01571e81,
	0x01571f01,

	/* NID 0x16, Port F, Rear panel surround-back (5.1) jack */
	0x01671c12,
	0x01671d10,
	0x01671e01,
	0x01671f01,

	/* NID 0x17, Port E, Rear panel microphone jack */
	0x01771c20,
	0x01771d90,
	0x01771ea1,
	0x01771f01,

	/* NID 0x18, CD IN, Analog CD input */
	0x01871c2e,
	0x01871d11,
	0x01871e33,
	0x01871f99,

	/* NID 0x1a, Analog PCBEEP, External analog PCBEEP signal input */
	0x01a71cf0,
	0x01a71d01,
	0x01a71ef3,
	0x01a71f99,

	/* NID 0x1b, S/PDIF Out, S/PDIF output pin */
	0x01b71cf0,
	0x01b71df1,
	0x01b71e45,
	0x01b71f01,

	/* NID 0x1c, S/PDIF In, S/PDIF input pin */
	0x01c71cf0,
	0x01c71df1,
	0x01c71ec5,
	0x01c71f41,

	/* NID 0x24, Port G, Rear panel C/LFE jack */
	0x02471c11,
	0x02471d60,
	0x02471e01,
	0x02471f01,

	/* NID 0x25, Port H, Rear panel surround-side (7.1) jack */
	0x02571c14,
	0x02571d20,
	0x02571e01,
	0x02571f01,
};

extern const u32 *cim_verb_data;
extern u32 cim_verb_data_size;

