/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

static const u32 realtek_alc892_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x1849c892),
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x14, 0x01014020),
	AZALIA_PIN_CFG(0, 0x17, 0x90170110),
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, 0x02a11c3f),
	AZALIA_PIN_CFG(0, 0x1b, 0x01813c30),
	AZALIA_PIN_CFG(0, 0x1d, 0x598301f0),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x21, 0x0221102f),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC892",
		.vendor_id    = 0x10ec0892,
		.subsystem_id = 0x1849c892,
		.address      = 0,
		.verbs        = realtek_alc892_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc892_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
