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
	0x10438444, /* Subsystem ID */

	0x0000000f, /* Number of 4 dword sets */
	/* NID 0x01: Subsystem ID.  */
	AZALIA_SUBVENDOR(0x0, 0x10438444),

	/* NID 0x11.  */
	AZALIA_PIN_CFG(0x0, 0x11, 0x99430140),

	/* NID 0x12.  */
	AZALIA_PIN_CFG(0x0, 0x12, 0x411111f0),

	/* NID 0x14.  */
	AZALIA_PIN_CFG(0x0, 0x14, 0x01014010),

	/* NID 0x15.  */
	AZALIA_PIN_CFG(0x0, 0x15, 0x01011012),

	/* NID 0x16.  */
	AZALIA_PIN_CFG(0x0, 0x16, 0x01016011),

	/* NID 0x17.  */
	AZALIA_PIN_CFG(0x0, 0x17, 0x01012014),

	/* NID 0x18.  */
	AZALIA_PIN_CFG(0x0, 0x18, 0x01a19850),

	/* NID 0x19.  */
	AZALIA_PIN_CFG(0x0, 0x19, 0x02a19c60),

	/* NID 0x1a.  */
	AZALIA_PIN_CFG(0x0, 0x1a, 0x0181305f),

	/* NID 0x1b.  */
	AZALIA_PIN_CFG(0x0, 0x1b, 0x02214c20),

	/* NID 0x1c.  */
	AZALIA_PIN_CFG(0x0, 0x1c, 0x411111f0),

	/* NID 0x1d.  */
	AZALIA_PIN_CFG(0x0, 0x1d, 0x4005e601),

	/* NID 0x1e.  */
	AZALIA_PIN_CFG(0x0, 0x1e, 0x01456130),

	/* NID 0x1f.  */
	AZALIA_PIN_CFG(0x0, 0x1f, 0x411111f0),
	0x80862805, /* Codec Vendor / Device ID: Intel */
	0x80860101, /* Subsystem ID */

	0x00000004, /* Number of 4 dword sets */
	/* NID 0x01: Subsystem ID.  */
	AZALIA_SUBVENDOR(0x3, 0x80860101),

	/* NID 0x05.  */
	AZALIA_PIN_CFG(0x3, 0x05, 0x58560010),

	/* NID 0x06.  */
	AZALIA_PIN_CFG(0x3, 0x06, 0x58560020),

	/* NID 0x07.  */
	AZALIA_PIN_CFG(0x3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
