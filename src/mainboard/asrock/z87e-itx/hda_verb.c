/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0900,	/* Codec Vendor / Device ID: Realtek */
	0x18491150,	/* Subsystem ID */
	11,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x18491150),
	AZALIA_PIN_CFG(0, 0x11, 0x40000000),
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, 0x01011012),
	AZALIA_PIN_CFG(0, 0x16, 0x01016011),
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19040),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19050),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181304f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214020),
	AZALIA_PIN_CFG(0, 0x1e, 0x01451130),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
