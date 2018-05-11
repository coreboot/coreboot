/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
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

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0269,	/* Codec Vendor / Device ID: Realtek ALC269 */
	0x19910269,	/* Subsystem ID */
	0x0000000c,	/* Number of jacks (NID entries) */

	0x0017ff00,	/* Function Reset */
	0x0017ff00,	/* Double Function Reset */
	0x0017ff00,
	0x0017ff00,

	/* Bits 31:28 - Codec Address */
	/* Bits 27:20 - NID */
	/* Bits 19:8 - Verb ID */
	/* Bits 7:0  - Payload */

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x19910269 */
	0x00172069,
	0x00172102,
	0x00172291,
	0x00172319,

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x12) */
	0x01271c00,
	0x01271d00,
	0x01271e00,
	0x01271f40,

	/* Pin Complex (NID 0x14) */
	0x01471c10,
	0x01471d01,
	0x01471e17,
	0x01471f90,

	/* Pin Complex (NID 0x17) */
	0x01771cf0,
	0x01771d11,
	0x01771e11,
	0x01771f41,

	/* Pin Complex (NID 0x18) */
	0x01871c20,
	0x01871d10,
	0x01871ea1,
	0x01871f04,

	/* Pin Complex (NID 0x19) */
	0x01971c30,
	0x01971d01,
	0x01971ea7,
	0x01971f90,

	/* Pin Complex (NID 0x1A) */
	0x01a71cf0,
	0x01a71d11,
	0x01a71e11,
	0x01a71f41,

	/* Pin Complex (NID 0x1B) */
	0x01b71cf0,
	0x01b71d11,
	0x01b71e11,
	0x01b71f41,

	/* Pin Complex (NID 0x1D) */
	0x01d71c05,
	0x01d71d9d,
	0x01d71e56,
	0x01d71f40,

	/* Pin Complex (NID 0x1E) */
	0x01e71cf0,
	0x01e71d11,
	0x01e71e11,
	0x01e71f41,

	/* Pin Complex (NID 0x21) */
	0x02171c1f,
	0x02171d10,
	0x02171e21,
	0x02171f04,
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
