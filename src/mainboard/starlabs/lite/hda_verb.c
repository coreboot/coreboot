/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0269,	/* Codec Vendor / Device ID: Realtek ALC269 */
	0x10ec111e,	/* Subsystem ID */
	15,		/* Number of jacks (NID entries) */

	/* Reset Codec First */
	AZALIA_RESET(0x1),

	/* HDA Codec Subsystem ID */
	AZALIA_SUBVENDOR(0, 0x10ec111e),

	/* Pin Widget Verb-table */
	AZALIA_PIN_CFG(0, 0x01, 0x00000000),
	AZALIA_PIN_CFG(0, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x14, 0x94171110),
	AZALIA_PIN_CFG(0, 0x15, 0x042b1010),
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x04ab1020),
	AZALIA_PIN_CFG(0, 0x19, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1a, 0x93171110),
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),

	0x00370600,
	0x00270600,
	0x00b707C0,
	0x00d70740,

	0x0017a200,
	0x0017c621,
	0x0017a208,
	0x00170500,

	0x8086280d,	/* Codec Vendor / Device ID: Intel */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of 4 dword sets */

	AZALIA_SUBVENDOR(2, 0x80860101),

	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560020),
	AZALIA_PIN_CFG(2, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[] = {
};

AZALIA_ARRAY_SIZES;
