/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10EC0298,	/* Codec Vendor - Device ID: Realtek ALC298 */
	0x152D1165,	/* Subsystem ID Quanta */
	0x0000000E,	/* Number of jacks */

	/* HDA Codec Subsystem ID Verb Table */
	AZALIA_SUBVENDOR(0, 0x152D1165),

	/* Pin Widget Verb Table */

	/* Widget node 1 (NID 0x01) */
	0x0017FF00,
	0x0017FF00,
	0x0017FF00,
	0x0017FF00,

	/* Pin Complex (NID 0x12) DMIC */
	AZALIA_PIN_CFG(0, 0x12, 0x90A60130),

	/* Pin Complex (NID 0x13) DMIC */
	AZALIA_PIN_CFG(0, 0x13, 0x411111F0),

	/* Pin Complex (NID 0x14) SPEAKER-OUT (Port-D) */
	AZALIA_PIN_CFG(0, 0x14, 0x90180110),

	/* Pin Complex (NID 0x17) I2S-OUT */
	AZALIA_PIN_CFG(0, 0x17, 0x01011120),

	/* Pin Complex (NID 0x18) MIC1 (Port-B) */
	AZALIA_PIN_CFG(0, 0x18, 0x411111F0),

	/* Pin Complex (NID 0x19) I2S-IN */
	AZALIA_PIN_CFG(0, 0x19, 0x90870140),

	/* Pin Complex (NID 0x1A) LINE1 (Port-C) */
	AZALIA_PIN_CFG(0, 0x1A, 0x411111F0),

	/* Pin Complex (NID 0x1D) PC-BEEP */
	AZALIA_PIN_CFG(0, 0x1D, 0x40400001),

	/* Pin Complex (NID 0x1E) SPDIF-OUT */
	AZALIA_PIN_CFG(0, 0x1E, 0x411111F0),

	/* Pin Complex (NID 0x1F) SPDIF-IN */
	AZALIA_PIN_CFG(0, 0x1F, 0x411111F0),

	/* Pin Complex (NID 0x21) HP-OUT (Port-A) */
	AZALIA_PIN_CFG(0, 0x21, 0x411111F0),

	/* POST I2S bypass output SRC */
	0x0205002D,
	0x0204C020,
	0x0205002D,
	0x0204C020,

};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
