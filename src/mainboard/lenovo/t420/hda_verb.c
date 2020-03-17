/*
 * This file is part of the coreboot project.
 *
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

/* Bits 31:28 - Codec Address */
/* Bits 27:20 - NID */
/* Bits 19:8 - Verb ID */
/* Bits 7:0  - Payload */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x14f1506e,	/* Codec VID / DID: Conexant CX20590 - schematic shows CX20672 */
	0x17aa21ce,	/* Subsystem ID */
	13,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x17aa21d2),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60140),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x15, 0x03211020),
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x03a11830),
	AZALIA_PIN_CFG(0, 0x19, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x40138205),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),

	/* Misc entries */
		0x00b707C0, /* Enable PortB as Output with HP amp */
		0x00d70740, /* Enable PortD as Output */
		0x0017a200, /* Disable ClkEn of PortSenseTst */
		0x0017c621, /* Slave Port - Port A used as microphone input for
		                            combo Jack
		               Master Port - Port B used for Jack Presence Detect
		               Enable Combo Jack Detection */
		0x0017a208, /* Enable ClkEn of PortSenseTst */
		0x00170500, /* Set power state to D0 */
		0x00170500, /* Padding */
		0x00170500, /* Padding */
};

const u32 pc_beep_verbs[] = {
	0x02177a00, /* Digital PCBEEP Gain: 0h=-9db, 1h=-6db ... 4h=+3db, 5h=+6db */
};

AZALIA_ARRAY_SIZES;
