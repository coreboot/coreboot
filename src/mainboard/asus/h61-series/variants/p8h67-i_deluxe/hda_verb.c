/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0892,	/* Codec Vendor / Device ID: Realtek */
	0x1043848d,	/* Subsystem ID */
	15,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x1043848d),
	AZALIA_PIN_CFG(0, 0x11, 0x99430140),
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19850),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19c60),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181305f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214c20),
	AZALIA_PIN_CFG(0, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x4004c601),
	AZALIA_PIN_CFG(0, 0x1e, 0x01452130),
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0)),

	0x80862805,	/* Codec Vendor / Device ID: Intel */
	0x80862805,	/* Subsystem ID */
	4,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(3, 0x80862805),
	AZALIA_PIN_CFG(3, 0x05, 0x58560010),
	AZALIA_PIN_CFG(3, 0x06, 0x58560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),

};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
