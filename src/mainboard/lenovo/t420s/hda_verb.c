/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/*	Vendor Name    : Conexant
 *	Vendor ID      : 0x14f1506e
 *	Subsystem ID   : 0x17aa21d2
 *	Revision ID    : 0x100002
 */


#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x14f1506e,	// Codec Vendor / Device ID: Conexant CX20590 - Schematic show CX20672
	0x17aa21d2,	// Subsystem ID
	0x0000000d,	// Number of 4 dword sets

/* Bits 31:28 - Codec Address */
/* Bits 27:20 - NID */
/* Bits 19:8 - Verb ID */
/* Bits 7:0  - Payload */

/* NID 0x01 - NodeInfo */
	AZALIA_SUBVENDOR(0x0, 0x17AA21D2),

	AZALIA_PIN_CFG(0x0, 0x19, 0x04211040),
	AZALIA_PIN_CFG(0x0, 0x1A, 0x61A19050),
	AZALIA_PIN_CFG(0x0, 0x1B, 0x04A11060),
	AZALIA_PIN_CFG(0x0, 0x1C, 0x6121401F),
	AZALIA_PIN_CFG(0x0, 0x1D, 0x40F001F0),
	AZALIA_PIN_CFG(0x0, 0x1E, 0x40F001F0),
	AZALIA_PIN_CFG(0x0, 0x1F, 0x90170110),
	AZALIA_PIN_CFG(0x0, 0x20, 0x40F001F0),
	AZALIA_PIN_CFG(0x0, 0x22, 0x40F001F0),
	AZALIA_PIN_CFG(0x0, 0x23, 0x90A60170),

	/* Misc entries */
		0x00B707C0, /* Enable PortB as Output with HP amp */
		0x00D70740, /* Enable PortD as Output */
		0x0017A200, /* Disable ClkEn of PortSenseTst */
		0x0017C621, /* Slave Port - Port A used as microphone input for
		                            combo Jack
		               Master Port - Port B used for Jack Presence Detect
		               Enable Combo Jack Detection */
		0x0017A208, /* Enable ClkEn of PortSenseTst */
		0x00170500, /* Set power state to D0 */
		0x00170500, /* Padding */
		0x00170500, /* Padding */
};

const u32 pc_beep_verbs[] = {
	0x02177a00, /* Digital PCBEEP Gain: 0h=-9db, 1h=-6db ... 4h=+3db, 5h=+6db */
};

AZALIA_ARRAY_SIZES;
