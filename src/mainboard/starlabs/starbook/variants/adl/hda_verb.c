/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <stdint.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0269, /* Codec Vendor / Device ID: Realtek ALC269 */
	0x1e507007, /* Subsystem ID */
	36,	    /* Number of jacks (NID entries) */

	/* Reset Codec First */
	AZALIA_RESET(0x1),

	/* HDA Codec Subsystem ID Verb-table */
	AZALIA_SUBVENDOR(0, 0x1e507007),

	/* Pin Widget Verb-table */
	AZALIA_PIN_CFG(0, 0x01, 0x00000000),
	AZALIA_PIN_CFG(0, 0x12, 0x90a61120),
	AZALIA_PIN_CFG(0, 0x14, 0x90171110),
	AZALIA_PIN_CFG(0, 0x17, 0x40000000),
	AZALIA_PIN_CFG(0, 0x18, 0x04a19030),
	AZALIA_PIN_CFG(0, 0x19, 0x04ab1020),
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x21, 0x042b1010),

	/* ALC269 Default 1 */
	0x02050011,
	0x02041410,
	0x02050012,
	0x02042901,

	/* ALC269 Default 2 */
	0x0205000d,
	0x02044440,
	0x02050007,
	0x02040040,

	/* ALC269 Default 3 */
	0x02050002,
	0x0204aab8,
	0x02050008,
	0x02040300,

	/* ALC269 Default 4 */
	0x02050017,
	0x020400af,
	0x02050005,
	0x020400c0,

	0x80862815, /* Codec Vendor / Device ID: Intel */
	0x80860101, /* Subsystem ID */
	9,	    /* Number of 4 dword sets */

	AZALIA_SUBVENDOR(2, 0x80860101),

	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x08, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0a, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0b, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0c, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0d, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0e, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0f, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
