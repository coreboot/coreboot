/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x14f15069,	/* Codec Vendor / Device ID: Conexant CX20585 */
	0x17aa2155,	/* Subsystem ID */
	12,		/* Number of jacks (NID entries) */

	0x0017ff00,	/* Function Reset */
	0x0017ff00,	/* Double Function Reset */
	0x0017ff00,
	0x0017ff00,

	/* Bits 31:28 - Codec Address */
	/* Bits 27:20 - NID */
	/* Bits 19:8 - Verb ID */
	/* Bits 7:0  - Payload */

	/* NID 0x01, HDA Codec Subsystem ID */
	AZALIA_SUBVENDOR(0, 0x17aa2155),

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(0, 0x19, 0x042140f0),
	AZALIA_PIN_CFG(0, 0x1a, 0x61a190f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x04a190f0),
	AZALIA_PIN_CFG(0, 0x1c, 0x612140f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x601700f0),
	AZALIA_PIN_CFG(0, 0x1e, 0x40f001f0),
	AZALIA_PIN_CFG(0, 0x1f, 0x901701f0),
	AZALIA_PIN_CFG(0, 0x1B, 0x40f001f0),
	AZALIA_PIN_CFG(0, 0x22, 0x40f001f0),
	AZALIA_PIN_CFG(0, 0x23, 0x90a601f0),
};

const u32 pc_beep_verbs[] = {
};
AZALIA_ARRAY_SIZES;
