/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
	0x10ec0262,	// Codec Vendor / Device ID: Realtek ALC262
	0x10714700,	// Subsystem ID
	0x0000000d,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x10ec0000 */
	AZALIA_SUBVENDOR(0x0, 0x10EC0000),

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x12), DMIC */
	AZALIA_PIN_CFG(0x0, 0x12, 0x411111F0),

	/* Pin Complex (NID 0x14), LINE_OUT (port D) */
	AZALIA_PIN_CFG(0x0, 0x14, 0x99130110),

	/* Pin Complex (NID 0x15), HP_OUT (port A) */
	AZALIA_PIN_CFG(0x0, 0x15, 0x01214020),

	/* Pin Complex (NID 0x16), MONO-OUT */
	AZALIA_PIN_CFG(0x0, 0x16, 0x411111F0),

	/* Pin Complex (NID 0x18), MIC1 (port B) */
	AZALIA_PIN_CFG(0x0, 0x18, 0x01A19830),

	/* Pin Complex (NID 0x19), MIC2 (port F) */
	AZALIA_PIN_CFG(0x0, 0x19, 0x99A30931),

	/* Pin Complex (NID 0x1A), LINE1 (port C) */
	AZALIA_PIN_CFG(0x0, 0x1A, 0x02A1983F),

	/* Pin Complex (NID 0x1B), LINE2 (port E) */
	AZALIA_PIN_CFG(0x0, 0x1B, 0x0221401F),

	/* Pin Complex (NID 0x1C), CD_IN */
	AZALIA_PIN_CFG(0x0, 0x1C, 0x411111F0),

	/* Pin Complex (NID 0x1D), PCBEEP */
	AZALIA_PIN_CFG(0x0, 0x1D, 0x411111F0),

	/* Pin Complex (NID 0x1E), S/PDIF-OUT */
	AZALIA_PIN_CFG(0x0, 0x1E, 0x411111F0),

	/* Pin Complex (NID 0x1F), S/PDIF-IN */
	AZALIA_PIN_CFG(0x0, 0x1F, 0x411111F0),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
