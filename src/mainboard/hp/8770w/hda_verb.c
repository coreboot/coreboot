/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
 * Copyright (C) 2017 Iru Cai <mytbk920423@gmail.com>
 * Copyright (C) 2018 Robert Reeves
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
	0x111d7605, /* Codec Vendor / Device ID: IDT */
	0x103c176c, /* Subsystem ID */

	0x0000000b, /* Number of 4 dword sets */
	/* NID 0x01: Subsystem ID.  */
	AZALIA_SUBVENDOR(0x0, 0x103c176c),

	/* NID 0x0a.  */
	AZALIA_PIN_CFG(0x0, 0x0a, 0x21011030),

	/* NID 0x0b.  */
	AZALIA_PIN_CFG(0x0, 0x0b, 0x0421101f),

	/* NID 0x0c.  */
	AZALIA_PIN_CFG(0x0, 0x0c, 0x04a11020),

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
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
