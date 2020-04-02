/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0292,	/* Codec Vendor / Device ID: Realtek */
	0x17aa220e,	/* Subsystem ID */
	12,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x17aa220e),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60130),
	AZALIA_PIN_CFG(0, 0x13, 0x40000000),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x15, 0x0321101f),
	AZALIA_PIN_CFG(0, 0x16, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x19, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1a, 0x03a11020),
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x40738105),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
