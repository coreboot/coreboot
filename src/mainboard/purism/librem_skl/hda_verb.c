/*
 * This file is part of the coreboot project.
 *
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
	AZALIA_SUBVENDOR(0, 0x19910269),

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x12) */
	AZALIA_PIN_CFG(0, 0x12, 0x40000000),

	/* Pin Complex (NID 0x14) */
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),

	/* Pin Complex (NID 0x15) */
	AZALIA_PIN_CFG(0, 0x15, 0x04214020),

	/* Pin Complex (NID 0x17) */
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0),

	/* Pin Complex (NID 0x18) */
	AZALIA_PIN_CFG(0, 0x18, 0x04a19040),

	/* Pin Complex (NID 0x19) */
	AZALIA_PIN_CFG(0, 0x19, 0x90a70130),

	/* Pin Complex (NID 0x1A) */
	AZALIA_PIN_CFG(0, 0x1A, 0x411111f0),

	/* Pin Complex (NID 0x1B) */
	AZALIA_PIN_CFG(0, 0x1B, 0x411111f0),

	/* Pin Complex (NID 0x1D) */
	AZALIA_PIN_CFG(0, 0x1D, 0x40548505),

	/* Pin Complex (NID 0x1E) */
	AZALIA_PIN_CFG(0, 0x1E, 0x411111f0),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
