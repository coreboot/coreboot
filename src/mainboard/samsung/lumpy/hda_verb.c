/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
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
	0x10134210,	// Codec Vendor / Device ID: Realtek ALC262
	0x144db082,	// Subsystem ID
	0x00000007,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x144dc0c2 */
	AZALIA_SUBVENDOR(0x0, 0x144db082),

	0x00170500,

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x05) */
	AZALIA_PIN_CFG(0x0, 0x05, 0x022110f0),

	/* Pin Complex (NID 0x06) */
	AZALIA_PIN_CFG(0x0, 0x06, 0x901700f0),

	/* Pin Complex (NID 0x07) */
	AZALIA_PIN_CFG(0x0, 0x07, 0x02a110f0),

	/* Pin Complex (NID 0x08) */
	AZALIA_PIN_CFG(0x0, 0x08, 0x77a70037),

	/* Pin Complex (NID 0x09) */
	AZALIA_PIN_CFG(0x0, 0x09, 0xb7a6003e),

	/* Pin Complex (NID 0x0a) */
	AZALIA_PIN_CFG(0x0, 0x0a, 0x434510f0)
};

const u32 pc_beep_verbs[] = {
	0x00170500,			/* power up codec */
	0x00270500,			/* power up DAC */
	0x00670500,			/* power up speaker */
	0x00670740,			/* enable speaker output */
	0x0023B04B,			/* set DAC gain */
};
AZALIA_ARRAY_SIZES;
