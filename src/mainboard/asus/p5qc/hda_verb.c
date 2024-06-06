/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0888,
	0x104382fe,	// Subsystem ID
	13,	// Number of entries

	/* Pin Widget Verb Table */

	AZALIA_PIN_CFG(0, 0x11, 0x99430140),
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, 0x01011012),
	AZALIA_PIN_CFG(0, 0x16, 0x01016011),
	AZALIA_PIN_CFG(0, 0x17, 0x01012014),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19850),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19d60),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181305f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214d20),
	AZALIA_PIN_CFG(0, 0x1c, 0x593301f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x4015e601),
	AZALIA_PIN_CFG(0, 0x1e, 0x01447130),
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0)),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
