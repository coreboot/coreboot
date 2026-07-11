/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc662_verbs[] = {
	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(0, 0x14, 0x01014c10),
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19c30),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19c31),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181343f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214c1f),
	AZALIA_PIN_CFG(0, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x4005c603),
	AZALIA_PIN_CFG(0, 0x1e, 0x99430120),
};

const u32 pc_beep_verbs[] = {
};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC662",
		.vendor_id    = 0x10ec0662,
		.subsystem_id = 0x105b0d55,
		.address      = 0,
		.verbs        = realtek_alc662_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc662_verbs),
	},
};

AZALIA_ARRAY_SIZES;
