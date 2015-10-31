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
	0x10ec0272,	/* Codec Vendor / Device ID: Realtek ALC272X */
	0x10250379,	/* Subsystem ID  */
	0x00000006,	/* Number of 4 dword sets */

	/* NID 0x01: Subsystem ID.  */
	AZALIA_SUBVENDOR(0x0, 0x10250379),

	/* NID 0x14.  */
	AZALIA_PIN_CFG(0x0, 0x14, 0x99130110),

	/* NID 0x18.  */
	AZALIA_PIN_CFG(0x0, 0x18, 0x03A11830),

	/* NID 0x19.  */
	AZALIA_PIN_CFG(0x0, 0x19, 0x99A30920),

	/* NID 0x1D.  */
	AZALIA_PIN_CFG(0x0, 0x1D, 0x4017992D),

	/* NID 0x21.  */
	AZALIA_PIN_CFG(0x0, 0x21, 0x0321101F),

	0x80862804,	/* Codec Vendor / Device ID: Intel Ibexpeak HDMI.  */
	0x80860101,	/* Subsystem ID  */
	0x00000004,	/* Number of 4 dword sets */

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x17aa21b5 */
	AZALIA_SUBVENDOR(0x3, 0x80860101),

	/* NID 0x04.  */
	AZALIA_PIN_CFG(0x3, 0x04, 0x18560010),

	/* NID 0x05.  */
	AZALIA_PIN_CFG(0x3, 0x05, 0x58560020),

	/* NID 0x06.  */
	AZALIA_PIN_CFG(0x3, 0x06, 0x58560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
