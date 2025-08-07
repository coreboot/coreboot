/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc662_verbs[] = {
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19840),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19850),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181304f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214020),
	AZALIA_PIN_CFG(0, 0x1c, 0x593301f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x4005c603),
	AZALIA_PIN_CFG(0, 0x1e, 0x18561130),
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0)),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC662",
		.vendor_id    = 0x10ec0883,
		.subsystem_id = 0x104382c7,
		.address      = 0,
		.verbs        = realtek_alc662_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc662_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
