/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0662, /* Codec Vendor / Device ID: Realtek */
	0x103c1495, /* Subsystem ID */

	0x0000000b, /* Number of 4 dword sets */
	/* NID 0x01: Subsystem ID.  */
	AZALIA_SUBVENDOR(0x0, 0x103c1495),

	/* NID 0x14.  */
	AZALIA_PIN_CFG(0x0, 0x14, 0x01014010),

	/* NID 0x15.  */
	AZALIA_PIN_CFG(0x0, 0x15, 0x99130120),

	/* NID 0x16.  */
	AZALIA_PIN_CFG(0x0, 0x16, 0x411111f0),

	/* NID 0x18.  */
	AZALIA_PIN_CFG(0x0, 0x18, 0x01813c30),

	/* NID 0x19.  */
	AZALIA_PIN_CFG(0x0, 0x19, 0x02a11c3f),

	/* NID 0x1a.  */
	AZALIA_PIN_CFG(0x0, 0x1a, 0x411111f0),

	/* NID 0x1b.  */
	AZALIA_PIN_CFG(0x0, 0x1b, 0x0221101f),

	/* NID 0x1c.  */
	AZALIA_PIN_CFG(0x0, 0x1c, 0x411111f0),

	/* NID 0x1d.  */
	AZALIA_PIN_CFG(0x0, 0x1d, 0x40028101),

	/* NID 0x1e.  */
	AZALIA_PIN_CFG(0x0, 0x1e, 0x411111f0),
	0x80862805, /* Codec Vendor / Device ID: Intel */
	0x80861495, /* Subsystem ID */

	0x00000004, /* Number of 4 dword sets */
	/* NID 0x01: Subsystem ID.  */
	AZALIA_SUBVENDOR(0x3, 0x80861495),

	/* NID 0x05.  */
	AZALIA_PIN_CFG(0x3, 0x05, 0x58560010),

	/* NID 0x06.  */
	AZALIA_PIN_CFG(0x3, 0x06, 0x18560020),

	/* NID 0x07.  */
	AZALIA_PIN_CFG(0x3, 0x07, 0x58560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
