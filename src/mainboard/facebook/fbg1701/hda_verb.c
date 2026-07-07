/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc298_verbs[] = {
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

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC298",
		.vendor_id    = 0x10EC0298,
		.subsystem_id = 0x152D1165,	/* Quanta */
		.address      = 0,
		.verbs        = realtek_alc298_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc298_verbs),
	},
};

AZALIA_ARRAY_SIZES;
