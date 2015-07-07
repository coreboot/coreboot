/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Copyright (C) 2006-2007 AMD
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
	0x10ec0880,	// Codec Vendor / Device ID: Realtek ALC880
	0x00000000,	// Subsystem ID
	0x0000000d,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x0000e601 */
	AZALIA_SUBVENDOR(0x0, 0x0000e601),

	/* NID 0x14, FRONT-OUT-L/R */
	AZALIA_PIN_CFG(0x0, 0x14, 0x01014410),

	/* NID 0x15, SURR-OUT-L/R */
	AZALIA_PIN_CFG(0x0, 0x15, 0x01011412),

	/* NID 0x16, CEN/LFE-OUT */
	AZALIA_PIN_CFG(0x0, 0x16, 0x01016011),

	/* NID 0x17, SIDE-SURR-L/R */
	AZALIA_PIN_CFG(0x0, 0x17, 0x01012014),

	/* NID 0x18, MIC1-L/R, VREFO */
	AZALIA_PIN_CFG(0x0, 0x18, 0x01a19c30),

	/* NID 0x19, MIC2-L/R, VREFO */
	AZALIA_PIN_CFG(0x0, 0x19, 0x02a19c40),

	/* NID 0x1a, LINE1-L/R, VREFO */
	AZALIA_PIN_CFG(0x0, 0x1a, 0x01813431),

	/* NID 0x1b, LINE2-L/R, VREFO */
	AZALIA_PIN_CFG(0x0, 0x1b, 0x0221441f),

	/* NID 0x1c, CD-L/R / GND */
	AZALIA_PIN_CFG(0x0, 0x1c, 0x411111f0),

	/* NID 0x1d, PCBEEP */
	AZALIA_PIN_CFG(0x0, 0x1d, 0x9983013e),

	/* NID 0x1e, S/PDIF-OUT */
	AZALIA_PIN_CFG(0x0, 0x1e, 0x01454120),

	/* NID 0x1f, S/PDIF-IN */
	AZALIA_PIN_CFG(0x0, 0x1f, 0x01c59150),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
