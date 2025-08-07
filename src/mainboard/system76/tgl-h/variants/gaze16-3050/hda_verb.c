/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc256_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x15585017),
	AZALIA_RESET(1),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60130),
	AZALIA_PIN_CFG(0, 0x13, 0x40000000),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1b, 0x02a11040),
	AZALIA_PIN_CFG(0, 0x1d, 0x41700001),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x21, 0x02211020),
};

const u32 pc_beep_verbs[] = {
	// Adjust mic coefficient
	0x02050007,
	0x02040202,
};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC256",
		.vendor_id    = 0x10ec0256,
		.subsystem_id = 0x15585017,
		.address      = 0,
		.verbs        = realtek_alc256_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc256_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
