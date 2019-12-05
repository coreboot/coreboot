/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Vladimir Serbinenko
 * Copyright (C) 2017 Hal Martin <hal.martin@gmail.com>
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
	0x10ec0888,	/* Codec Vendor / Device ID: Realtek */
	0x10ec0888,	/* Subsystem ID */
	15,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0x0, 0x10ec0888),
	AZALIA_PIN_CFG(0x0, 0x11, 0x411110f0),
	AZALIA_PIN_CFG(0x0, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x14, 0x01214120),
	AZALIA_PIN_CFG(0x0, 0x15, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x16, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x18, 0x01a19131),
	AZALIA_PIN_CFG(0x0, 0x19, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x1a, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x1b, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x1c, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x1d, 0x411111f0),
	AZALIA_PIN_CFG(0x0, 0x1e, 0x014421f0),
	AZALIA_PIN_CFG(0x0, 0x1f, 0x01c421f0),

	0x80862806,	/* Codec Vendor / Device ID: Intel */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0x3, 0x80860101),
	AZALIA_PIN_CFG(0x3, 0x05, 0x58560010),
	AZALIA_PIN_CFG(0x3, 0x06, 0x18560020),
	AZALIA_PIN_CFG(0x3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
