/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0662,
	0x105b0d55,	/* Subsystem ID */
	10,		/* Number of entries */

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(0, 0x14, 0x01014c10),
	AZALIA_PIN_CFG(0, 0x15, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x16, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19c30),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19c31),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181343f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214c1f),
	AZALIA_PIN_CFG(0, 0x1c, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x4005c603),
	AZALIA_PIN_CFG(0, 0x1e, 0x99430120),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
