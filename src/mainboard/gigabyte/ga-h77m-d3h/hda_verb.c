/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x11060441,	/* Codec Vendor / Device ID: VIA VT2020 */
	0x1458a014,	/* Subsystem ID */
	13,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(2, 0x1458a014),
	AZALIA_PIN_CFG(2, 0x24, 0x01014010),
	AZALIA_PIN_CFG(2, 0x25, 0x410110f0),
	AZALIA_PIN_CFG(2, 0x26, 0x410160f0),
	AZALIA_PIN_CFG(2, 0x27, 0x410120f0),
	AZALIA_PIN_CFG(2, 0x28, 0x0221401f),
	AZALIA_PIN_CFG(2, 0x29, 0x02a19037),
	AZALIA_PIN_CFG(2, 0x2a, 0x0181303e),
	AZALIA_PIN_CFG(2, 0x2b, 0x01a19036),
	AZALIA_PIN_CFG(2, 0x2c, 0x503701f0),
	AZALIA_PIN_CFG(2, 0x2d, 0x474511f0),
	AZALIA_PIN_CFG(2, 0x2e, 0x074521f0),
	AZALIA_PIN_CFG(2, 0x2f, 0x47c521f0),

	0x80862806,	/* Codec Vendor / Device ID: Intel */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x58560010),
	AZALIA_PIN_CFG(3, 0x06, 0x58560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
