/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Angel Pons <th3fanbus@gmail.com>
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
	0x10ec0887, /* Codec Vendor / Device ID: Realtek */
	0x1458a002, /* Subsystem ID */

	0x0000000f, /* Number of 4 dword sets */
	/* NID 0x01: Subsystem ID.  */
	AZALIA_SUBVENDOR(0x2, 0x1458a002),

	/* NID 0x11.  */
	AZALIA_PIN_CFG(0x2, 0x11, 0x411111f0),

	/* NID 0x12.  */
	AZALIA_PIN_CFG(0x2, 0x12, 0x411111f0),

	/* NID 0x14.  */
	AZALIA_PIN_CFG(0x2, 0x14, 0x01014410),

	/* NID 0x15.  */
	AZALIA_PIN_CFG(0x2, 0x15, 0x411111f0),

	/* NID 0x16.  */
	AZALIA_PIN_CFG(0x2, 0x16, 0x411111f0),

	/* NID 0x17.  */
	AZALIA_PIN_CFG(0x2, 0x17, 0x411111f0),

	/* NID 0x18.  */
	AZALIA_PIN_CFG(0x2, 0x18, 0x01a19c50),

	/* NID 0x19.  */
	AZALIA_PIN_CFG(0x2, 0x19, 0x02a19c60),

	/* NID 0x1a.  */
	AZALIA_PIN_CFG(0x2, 0x1a, 0x0181345f),

	/* NID 0x1b.  */
	AZALIA_PIN_CFG(0x2, 0x1b, 0x02214c20),

	/* NID 0x1c.  */
	AZALIA_PIN_CFG(0x2, 0x1c, 0x411111f0),

	/* NID 0x1d.  */
	AZALIA_PIN_CFG(0x2, 0x1d, 0x4004c601),

	/* NID 0x1e.  */
	AZALIA_PIN_CFG(0x2, 0x1e, 0x411111f0),

	/* NID 0x1f.  */
	AZALIA_PIN_CFG(0x2, 0x1f, 0x411111f0),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
