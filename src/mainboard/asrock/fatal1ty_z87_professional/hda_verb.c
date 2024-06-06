/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x11020011,	/* Codec Vendor / Device ID: Creative */
	0x18491020,	/* Subsystem ID */
	11,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x18491020),
	AZALIA_PIN_CFG(0, 0x0b, 0x01014010),
	AZALIA_PIN_CFG(0, 0x0c, 0x014580f0),
	AZALIA_PIN_CFG(0, 0x0d, 0x014570f0),
	AZALIA_PIN_CFG(0, 0x0e, 0x01c530f0),
	AZALIA_PIN_CFG(0, 0x0f, 0x0221401f),
	AZALIA_PIN_CFG(0, 0x10, 0x02216011),
	AZALIA_PIN_CFG(0, 0x11, 0x02012014),
	AZALIA_PIN_CFG(0, 0x12, 0x37a791f0),
	AZALIA_PIN_CFG(0, 0x13, 0x908700f0),
	AZALIA_PIN_CFG(0, 0x18, 0x500000f0),

};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
