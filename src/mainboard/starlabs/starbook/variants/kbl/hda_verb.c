/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0269, /* Codec Vendor / Device ID: Realtek ALC269 */
	0x10ec10d0, /* Subsystem ID */
	18,	/* Number of jacks (NID entries) */

	/* Reset Codec First */
	AZALIA_RESET(0x1),

	/* HDA Codec Subsystem ID Verb-table */
	AZALIA_SUBVENDOR(0, 0x10ec10d0),

	/* Pin Widget Verb-table */
	AZALIA_PIN_CFG(0, 0x01, 0x00000000),
	AZALIA_PIN_CFG(0, 0x12, 0x90a61120),
	AZALIA_PIN_CFG(0, 0x14, 0x90171110),
	AZALIA_PIN_CFG(0, 0x15, 0x04ab1020),
	AZALIA_PIN_CFG(0, 0x17, 0x411111F0),
	AZALIA_PIN_CFG(0, 0x18, 0x04AB1020),
	AZALIA_PIN_CFG(0, 0x19, 0x411111F0),
	AZALIA_PIN_CFG(0, 0x1A, 0x411111F0),
	AZALIA_PIN_CFG(0, 0x1B, 0x411111F0),
	AZALIA_PIN_CFG(0, 0x1D, 0x411111F0),
	AZALIA_PIN_CFG(0, 0x1E, 0x411111F0),

	/* ALC269 Default 1 */
	0x02050018,
	0x02043984,
	0x0205001C,
	0x02040800,

	/* ALC269 Default 2 */
	0x02050024,
	0x02040000,
	0x02050004,
	0x02040080,

	/* ALC269 Default 3 */
	0x02050008,
	0x02040300,
	0x0205000C,
	0x02043F00,

	/* ALC269 Default 4 */
	0x02050015,
	0x02048002,
	0x02050015,
	0x02048002,

	/* ALC269 Default 5 */
	0x00C37080,
	0x00270610,
	0x00D37080,
	0x00370610,

	0x8086280b, /* Codec Vendor / Device ID: Intel */
	0x80860101, /* Subsystem ID */
	4,	    /* Number of 4 dword sets */

	AZALIA_SUBVENDOR(2, 0x80860101),

	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x07, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
