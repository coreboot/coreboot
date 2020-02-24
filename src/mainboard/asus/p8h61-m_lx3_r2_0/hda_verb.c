/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x11060397,	/* Codec Vendor / Device ID: VIA VT1708S */
	0x10438415,	/* Subsystem ID */
	12,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x10438415),
	AZALIA_PIN_CFG(0, 0x19, 0x410110f0),
	AZALIA_PIN_CFG(0, 0x1a, 0x01a19036),
	AZALIA_PIN_CFG(0, 0x1b, 0x0181303e),
	AZALIA_PIN_CFG(0, 0x1c, 0x01014010),
	AZALIA_PIN_CFG(0, 0x1d, 0x0221401f),
	AZALIA_PIN_CFG(0, 0x1e, 0x02a19037),
	AZALIA_PIN_CFG(0, 0x1f, 0x503701f0),
	AZALIA_PIN_CFG(0, 0x20, 0x585600f0),
	AZALIA_PIN_CFG(0, 0x21, 0x474411f0),
	AZALIA_PIN_CFG(0, 0x22, 0x410160f0),
	AZALIA_PIN_CFG(0, 0x23, 0x410120f0),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
