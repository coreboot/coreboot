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
 * Foundation, Inc.
 */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0283,	// Codec Vendor / Device ID: Realtek ALC283
	0x10ec0283,	// Subsystem ID
	0x0000000b,	// Number of jacks (NID entries)

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x10ec0283 */
	AZALIA_SUBVENDOR(0x0, 0x10ec0283),

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x12) DMIC - interior mobile lid */
	AZALIA_PIN_CFG(0x0, 0x12, 0xb7a61010),

	/* Pin Complex (NID 0x14) SPKR-OUT PORTD */
	// group 1, front left/right
	// no connector, no jack detect
	// speaker out, analog
	// fixed function, internal
	AZALIA_PIN_CFG(0x0, 0x14, 0x90170110),

	/* Pin Complex (NID 0x17)  */
	AZALIA_PIN_CFG(0x0, 0x17, 0x411111f0),

	/* Pin Complex (NID 0x18)  MIC1 PORTB */
	// group 1, cap 1
	// black, jack detect
	// mic in, analog
	// connector, left panel
	AZALIA_PIN_CFG(0x0, 0x19, 0x03a71011),

	/* Pin Complex (NID 0x19)  MIC2 PORTF */
	AZALIA_PIN_CFG(0x0, 0x18, 0x411111f0),

	/* Pin Complex (NID 0x1A)  LINE1 PORTC */
	AZALIA_PIN_CFG(0x0, 0x1a, 0x411111f0),

	/* Pin Complex (NID 0x1B)  LINE2 PORTE */
	AZALIA_PIN_CFG(0x0, 0x1a, 0x411111f0),

	/* Pin Complex (NID 0x1d)  PCBeep */
	// eapd low on ex-amp, laptop, custom enable
	// mute spkr on hpout
	// pcbeep en able, checksum
	// no physical, internal
	AZALIA_PIN_CFG(0x0, 0x1d, 0x4015812d),

	/* Pin Complex (NID 0x1E)  SPDIF-OUT */
	AZALIA_PIN_CFG(0x0, 0x1e, 0x411111f0),

	/* Pin Complex (NID 0x21) HPOUT PORT-I */
	// group1,
	// black, jack detect
	// HPOut, 1/8 stereo
	// connector, left panel
	AZALIA_PIN_CFG(0x0, 0x21, 0x0321101f),

};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
