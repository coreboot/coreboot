/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc269vc_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x15588560),
	AZALIA_RESET(1),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60140),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x15, 0x02211020),
	AZALIA_PIN_CFG(0, 0x17, 0x40000000),
	AZALIA_PIN_CFG(0, 0x18, 0x02a11030),
	AZALIA_PIN_CFG(0, 0x19, 0x02a1103f),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x40f00001),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC269",
		.vendor_id    = 0x10ec0269,
		.subsystem_id = 0x15588560,
		.address      = 0,
		.verbs        = realtek_alc269vc_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc269vc_verbs),
	},
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
