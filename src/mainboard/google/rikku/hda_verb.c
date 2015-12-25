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
 */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0283,	// Codec Vendor / Device ID: Realtek ALC283
	0x10ec0283,	// Subsystem ID
	0x0000000c,	// Number of jacks (NID entries)

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x10ec0283 */
	0x00172083,
	0x00172102,
	0x001722ec,
	0x00172310,

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x12) DMIC */
	0x01271cf0,
	0x01271d11,
	0x01271e11,
	0x01271f41,

	/* Pin Complex (NID 0x14) SPKR-OUT PORTD */
	0x01471cf0,
	0x01471d11,
	0x01471e11,
	0x01471f40,

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

	/* Undocumented settings from Realtek (needed for beep_gen) */
	/* Widget node 0x20 */
	0x02050010,
	0x02040c20,
	0x0205001b,
	0x0204081b,
};

const u32 pc_beep_verbs[] = {
	0x00170500,	/* power up everything (codec, dac, adc, mixers)  */
	0x01470740,	/* enable speaker out */
	0x01470c02,	/* set speaker EAPD pin */
	0x0143b01f,	/* unmute speaker */
	0x00c37100,	/* unmute mixer nid 0xc input 1 */
	0x00b37410,	/* unmute mixer nid 0xb beep input and set volume */
};

AZALIA_ARRAY_SIZES;
