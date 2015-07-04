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
	/* coreboot specific header */
	0x14F15069,	/* Codec Vendor / Device ID: Conexant CX20585 */
	0x17AA214C,	/* Subsystem ID  */
	0x0000000B,	/* Number of 4 dword sets */

	AZALIA_SUBVENDOR(0x0, 0x17AA214C),
	AZALIA_PIN_CFG(0x0, 0x19, 0x042110F0),
	AZALIA_PIN_CFG(0x0, 0x1A, 0x61A190F0),
	AZALIA_PIN_CFG(0x0, 0x1B, 0x04A110F0),
	AZALIA_PIN_CFG(0x0, 0x1C, 0x612140F0),
	AZALIA_PIN_CFG(0x0, 0x1D, 0x40F001F0),
	AZALIA_PIN_CFG(0x0, 0x1E, 0x40F001F0),
	AZALIA_PIN_CFG(0x0, 0x1F, 0x901701F0),
	AZALIA_PIN_CFG(0x0, 0x20, 0x40F001F0),
	AZALIA_PIN_CFG(0x0, 0x22, 0x40F001F0),
	AZALIA_PIN_CFG(0x0, 0x23, 0x90A601F0),

	0x80862804,	/* Codec Vendor / Device ID: Intel Ibexpeak HDMI.  */
	0x17AA21B5,	/* Subsystem ID  */
	0x00000004,	/* Number of 4 dword sets */

	AZALIA_SUBVENDOR(0x3, 0x17AA21B5),
	AZALIA_PIN_CFG(0x3, 0x04, 0x18560010),
	AZALIA_PIN_CFG(0x3, 0x05, 0x18560020),
	AZALIA_PIN_CFG(0x3, 0x06, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
