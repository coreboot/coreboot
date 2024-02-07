/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x111d76e7,	/* Codec Vendor / Device ID: IDT */
	0x1028049a,	/* Subsystem ID */
	11,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x1028049a),
	AZALIA_PIN_CFG(0, 0x0a, 0x04a11020),
	AZALIA_PIN_CFG(0, 0x0b, 0x0421101f),
	AZALIA_PIN_CFG(0, 0x0c, 0x400000f0),
	AZALIA_PIN_CFG(0, 0x0d, 0x90170110),
	AZALIA_PIN_CFG(0, 0x0e, 0x23011050),
	AZALIA_PIN_CFG(0, 0x0f, 0x23a1102e),
	AZALIA_PIN_CFG(0, 0x10, 0x400000f3),
	AZALIA_PIN_CFG(0, 0x11, 0xd5a301a0),
	AZALIA_PIN_CFG(0, 0x1f, 0x400000f0),
	AZALIA_PIN_CFG(0, 0x20, 0x400000f0),

	0x80862805,	/* Codec Vendor / Device ID: Intel */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
