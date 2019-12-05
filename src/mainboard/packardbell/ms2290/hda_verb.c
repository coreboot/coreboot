/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Vladimir Serbinenko.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0272,	/* Codec Vendor / Device ID: Realtek ALC272X */
	0x10250379,	/* Subsystem ID  */
	6,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0x0, 0x10250379),
	AZALIA_PIN_CFG(0x0, 0x14, 0x99130110),
	AZALIA_PIN_CFG(0x0, 0x18, 0x03a11830),
	AZALIA_PIN_CFG(0x0, 0x19, 0x99a30920),
	AZALIA_PIN_CFG(0x0, 0x1d, 0x4017992d),
	AZALIA_PIN_CFG(0x0, 0x21, 0x0321101f),

	0x80862804,	/* Codec Vendor / Device ID: Intel Ibexpeak HDMI */
	0x80860101,	/* Subsystem ID  */
	4,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0x3, 0x80860101),
	AZALIA_PIN_CFG(0x3, 0x04, 0x18560010),
	AZALIA_PIN_CFG(0x3, 0x05, 0x58560020),
	AZALIA_PIN_CFG(0x3, 0x06, 0x58560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
