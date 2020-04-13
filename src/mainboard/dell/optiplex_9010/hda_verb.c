/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0269,	/* Codec Vendor / Device ID: Realtek */
	0x1028052c,	/* Subsystem ID */

	11,		/* Number of 4 dword sets */

	AZALIA_SUBVENDOR(0, 0x1028052c),
	AZALIA_PIN_CFG(0, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x14, 0x99130110),
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x02a19830),
	AZALIA_PIN_CFG(0, 0x19, 0x01a19840),
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x01014020),
	AZALIA_PIN_CFG(0, 0x1d, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x21, 0x0221402f),

	0x80862806,	/* Codec Vendor / Device ID: Intel */
	0x80860101,	/* Subsystem ID */

	4,		/* Number of 4 dword sets */

	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),
	AZALIA_PIN_CFG(3, 0x07, 0x58560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
