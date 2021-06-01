/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _HDA_VERB_H_
#define _HDA_VERB_H_

#include <device/azalia_device.h>
#include <device/azalia.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0269,	// Codec Vendor / Device ID: Realtek ALC269
	0xffffffff,	// Subsystem ID
	0x0000002b,	// Number of jacks (NID entries)
	/* Rest Codec First */
	AZALIA_RESET(0x1),
	/* HDA Codec Subsystem ID Verb-table
	HDA Codec Subsystem ID  : 0x10EC111E */
	0x0017201E,
	0x00172111,
	0x001722EC,
	0x00172310,
	/* Pin Widget Verb-table */
	AZALIA_PIN_CFG(0, 0x01, 0x00000000),
	AZALIA_PIN_CFG(0, 0x12, 0x90a61120),
	AZALIA_PIN_CFG(0, 0x14, 0x90171110),
	AZALIA_PIN_CFG(0, 0x15, 0x042B1010),
	AZALIA_PIN_CFG(0, 0x17, 0x411111F0),
	AZALIA_PIN_CFG(0, 0x18, 0x04AB1020),
	AZALIA_PIN_CFG(0, 0x19, 0x411111F0),
	AZALIA_PIN_CFG(0, 0x1A, 0x411111F0),
	AZALIA_PIN_CFG(0, 0x1B, 0x411111F0),
	AZALIA_PIN_CFG(0, 0x1D, 0x411111F0),
	AZALIA_PIN_CFG(0, 0x1E, 0x411111F0),
	/* Widget node 0x20 */
	0x02050018,
	0x02040184, /* Stock: 0x02043984 */
	0x0205001C,
	0x02040800,
	/* Widget node 0x20 - 1 */
	0x02050024,
	0x02040000,
	0x02050004,
	0x02040080,
	/* Widget node 0x20 - 2 */
	0x02050008,
	0x02040300,
	0x0205000C,
	0x02043F00,
	/* Widget node 0x20 - 3 */
	0x02050015,
	0x02048002,
	0x02050015,
	0x02048002,
	/* Widget node 0x0C */
	0x00C37080,
	0x00270610,
	0x00D37080,
	0x00370610,

	0x8086280b,  /* Codec Vendor / Device ID: Intel */
	0x80860101,  /* Subsystem ID */
	0x00000004,  /* Number of 4 dword sets */

	AZALIA_SUBVENDOR(2, 0x80860101),

	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x07, 0x18560010),

};

const u32 pc_beep_verbs[] = {
};

AZALIA_ARRAY_SIZES;

#endif
