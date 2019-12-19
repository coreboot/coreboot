/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Nicola Corna <nicola@corna.info>
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
 */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0892,	/* Codec Vendor / Device ID: Realtek */
	0x10ec0000,	/* Subsystem ID */
	15,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(2, 0x10ec0000),
	AZALIA_PIN_CFG(2, 0x11, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x14, 0x01014c10),
	AZALIA_PIN_CFG(2, 0x15, 0x01011c12),
	AZALIA_PIN_CFG(2, 0x16, 0x01016c11),
	AZALIA_PIN_CFG(2, 0x17, 0x01012c14),
	AZALIA_PIN_CFG(2, 0x18, 0x01a19c40),
	AZALIA_PIN_CFG(2, 0x19, 0x02a19c50),
	AZALIA_PIN_CFG(2, 0x1a, 0x01813c4f),
	AZALIA_PIN_CFG(2, 0x1b, 0x0321403f),
	AZALIA_PIN_CFG(2, 0x1c, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x1d, 0x4005e601),
	AZALIA_PIN_CFG(2, 0x1e, 0x0145e130),
	AZALIA_PIN_CFG(2, 0x1f, 0x411111f0),

	0x80862805,	/* Codec Vendor / Device ID: Intel */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x58560010),
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
