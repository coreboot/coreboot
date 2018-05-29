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
	0x111d76e0, /* Codec Vendor / Device ID: IDT */
	0x103c18df, /* Subsystem ID */

	0x0000000b, /* Number of 4 dword sets */
	/* NID 0x01: Subsystem ID.  */
	AZALIA_SUBVENDOR(0x0, 0x103c18df),

	/* NID 0x0a.  */
	AZALIA_PIN_CFG(0x0, 0x0a, 0x21011030),

	/* NID 0x0b.  */
	AZALIA_PIN_CFG(0x0, 0x0b, 0x0321101f),

	/* NID 0x0c.  */
	AZALIA_PIN_CFG(0x0, 0x0c, 0x03a11020),

	/* NID 0x0d.  */
	AZALIA_PIN_CFG(0x0, 0x0d, 0x90170110),

	/* NID 0x0e.  */
	AZALIA_PIN_CFG(0x0, 0x0e, 0x40f000f0),

	/* NID 0x0f.  */
	AZALIA_PIN_CFG(0x0, 0x0f, 0x2181102e),

	/* NID 0x10.  */
	AZALIA_PIN_CFG(0x0, 0x10, 0x40f000f0),

	/* NID 0x11.  */
	AZALIA_PIN_CFG(0x0, 0x11, 0xd5a30140),

	/* NID 0x1f.  */
	AZALIA_PIN_CFG(0x0, 0x1f, 0x40f000f0),

	/* NID 0x20.  */
	AZALIA_PIN_CFG(0x0, 0x20, 0x40f000f0),
	0x80862806, /* Codec Vendor / Device ID: Intel */
	0x80860101, /* Subsystem ID */

	0x00000004, /* Number of 4 dword sets */
	/* NID 0x01: Subsystem ID.  */
	AZALIA_SUBVENDOR(0x3, 0x80860101),

	/* NID 0x05.  */
	AZALIA_PIN_CFG(0x3, 0x05, 0x18560010),

	/* NID 0x06.  */
	AZALIA_PIN_CFG(0x3, 0x06, 0x58560020),

	/* NID 0x07.  */
	AZALIA_PIN_CFG(0x3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
