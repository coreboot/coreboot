/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0888,
	0x80860028,	// Subsystem ID
	14,	// Number of entries

	/* Pin Widget Verb Table */

	AZALIA_PIN_CFG(0, 0x11, 0x01452140),
	AZALIA_PIN_CFG(0, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x14, 0x01014410),
	AZALIA_PIN_CFG(0, 0x15, 0x01011012),
	AZALIA_PIN_CFG(0, 0x16, 0x01016011),
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19850),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19960),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181345f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214520),
	AZALIA_PIN_CFG(0, 0x1c, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x4006f601),
	AZALIA_PIN_CFG(0, 0x1e, 0x99430130),
	AZALIA_PIN_CFG(0, 0x1f, 0x411111f0),

	/* HDMI */
	0x80862803,
	0x80860101,
	1,

	AZALIA_PIN_CFG(0, 0x03, 0x18560010)
};

const u32 pc_beep_verbs[0] = {};

const u32 pc_beep_verbs_size = ARRAY_SIZE(pc_beep_verbs);
const u32 cim_verb_data_size = ARRAY_SIZE(cim_verb_data);
