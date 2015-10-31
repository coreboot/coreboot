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
	0x17AA2155,	/* Subsystem ID  */
	0x0000000B,	/* Number of 4 dword sets */

	/* NID 0x01: Subsystem ID.  */
	AZALIA_SUBVENDOR(0x0, 0x17AA2155),

	/* NID 0x19: Headphone jack.  */
	AZALIA_PIN_CFG(0x0, 0x19, 0x042140F0),

	/* NID 0x1A: Dock mic jack.  */
	AZALIA_PIN_CFG(0x0, 0x1A, 0x61A190F0),

	/* NID 0x1B: Mic jack.  */
	AZALIA_PIN_CFG(0x0, 0x1B, 0x04A190F0),

	/* NID 0x1C: Dock headphone jack.  */
	AZALIA_PIN_CFG(0x0, 0x1C, 0x612140F0),

	/* NID 0x1D: EAPD detect.  */
	AZALIA_PIN_CFG(0x0, 0x1D, 0x601700F0),

	/* NID 0x1E  */
	AZALIA_PIN_CFG(0x0, 0x1E, 0x40F001F0),

	/* NID 0x1F  */
	AZALIA_PIN_CFG(0x0, 0x1F, 0x901701F0),

	/* NID 0x20  */
	AZALIA_PIN_CFG(0x0, 0x20, 0x40F001F0),

	/* NID 0x22  */
	AZALIA_PIN_CFG(0x0, 0x22, 0x40F001F0),

	/* NID 0x23: Internal mic boost volume.  */
	AZALIA_PIN_CFG(0x0, 0x23, 0x90A601F0),

	0x80862804,	/* Codec Vendor / Device ID: Intel Ibexpeak HDMI.  */
	0x17aa21b5,	/* Subsystem ID  */
	0x00000004,	/* Number of 4 dword sets */

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x17aa21b5 */
	AZALIA_SUBVENDOR(0x3, 0x17AA21B5),

	/* NID 0x04.  */
	AZALIA_PIN_CFG(0x3, 0x04, 0x58560010),

	/* NID 0x05.  */
	AZALIA_PIN_CFG(0x3, 0x05, 0x18560020),

	/* NID 0x06.  */
	AZALIA_PIN_CFG(0x3, 0x06, 0x58560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
