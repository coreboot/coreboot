/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
 * Copyright (C) 2017 Iru Cai <mytbk920423@gmail.com>
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
	0x111d7605,	/* Codec Vendor / Device ID: IDT */
	0x103c162a,	/* Subsystem ID */
	11,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0x0, 0x103c162a),
	AZALIA_PIN_CFG(0x0, 0x0a, 0x40f000f0),
	AZALIA_PIN_CFG(0x0, 0x0b, 0x0421401f),
	AZALIA_PIN_CFG(0x0, 0x0c, 0x04a11020),
	AZALIA_PIN_CFG(0x0, 0x0d, 0x90170110),
	AZALIA_PIN_CFG(0x0, 0x0e, 0x90a70130),
	AZALIA_PIN_CFG(0x0, 0x0f, 0x40f000f0),
	AZALIA_PIN_CFG(0x0, 0x10, 0x40f000f0),
	AZALIA_PIN_CFG(0x0, 0x11, 0x40f000f0),
	AZALIA_PIN_CFG(0x0, 0x1f, 0x40f000f0),
	AZALIA_PIN_CFG(0x0, 0x20, 0x40f000f0),

	0x11c11040,	/* Codec Vendor / Device ID: LSI */
	0x103c3066,	/* Subsystem ID */
	1,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0x1, 0x103c3066),

	0x80862805,	/* Codec Vendor / Device ID: Intel */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0x3, 0x80860101),
	AZALIA_PIN_CFG(0x3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(0x3, 0x06, 0x58560020),
	AZALIA_PIN_CFG(0x3, 0x07, 0x58560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
