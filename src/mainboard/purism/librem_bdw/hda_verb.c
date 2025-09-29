/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc269_verbs[] = {
	AZALIA_RESET(1),
	AZALIA_SUBVENDOR(0, 0x19910269),
	AZALIA_PIN_CFG(0, 0x12, 0x40000000),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x04a11020),
	AZALIA_PIN_CFG(0, 0x19, 0x90a70130),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x40569d05),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x21, 0x0421101f),
};

const u32 pc_beep_verbs[] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC269",
		.vendor_id    = 0x10ec0269,
		.subsystem_id = 0x19910269,
		.address      = 0,
		.verbs        = realtek_alc269_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc269_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
