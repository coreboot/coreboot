/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x11060397,	/* Codec Vendor / Device ID: VIA VT1708S */
	0x1043836c,	/* Subsystem ID */
	12,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x1043836c),
	AZALIA_PIN_CFG(0, 0x19, 0x01011012),
	AZALIA_PIN_CFG(0, 0x1a, 0x01a19036),
	AZALIA_PIN_CFG(0, 0x1b, 0x0181303e),
	AZALIA_PIN_CFG(0, 0x1c, 0x01014010),
	AZALIA_PIN_CFG(0, 0x1d, 0x0221401f),
	AZALIA_PIN_CFG(0, 0x1e, 0x02a19038),
	AZALIA_PIN_CFG(0, 0x1f, 0x503701f0),
	AZALIA_PIN_CFG(0, 0x20, 0x185600f0),
	AZALIA_PIN_CFG(0, 0x21, 0x074521f0),
	AZALIA_PIN_CFG(0, 0x22, 0x01016011),
	AZALIA_PIN_CFG(0, 0x23, 0x01012014),

	0x80862806,	/* Codec Vendor / Device ID: Intel HDMI */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(3, 0x06, 0x58560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),

};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
