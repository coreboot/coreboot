/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0221,	/* Codec Vendor / Device ID: Realtek */
	0x103c8000,	/* Subsystem ID */
	11,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x103c8000),
	AZALIA_PIN_CFG(0, 0x12, 0x400c0000),
	AZALIA_PIN_CFG(0, 0x14, 0x01014020),
	AZALIA_PIN_CFG(0, 0x17, 0x90170110),
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, 0x02a11030),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x40500001),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x21, 0x0221102f),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
