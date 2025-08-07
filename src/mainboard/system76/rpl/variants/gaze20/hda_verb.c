/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc255_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x15582560),
	AZALIA_RESET(1),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60130),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x17, 0x40000000),
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x41e79b45),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x21, 0x04211020),
};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name           = "Realtek ALC255",
		.vendor_id      = 0x10ec0255,
		.subsystem_id   = 0x15582560,
		.address        = 0,
		.verbs          = realtek_alc255_verbs,
		.verb_count     = ARRAY_SIZE(realtek_alc255_verbs),
	},
	{ /* terminator */ }
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
