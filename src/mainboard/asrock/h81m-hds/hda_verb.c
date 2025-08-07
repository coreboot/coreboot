/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <device/azalia_device.h>

static const u32 realtek_alc662_verbs[] = {
	AZALIA_SUBVENDOR(1, 0x18497662),
	AZALIA_PIN_CFG(1, 0x14, 0x01014010),
	AZALIA_PIN_CFG(1, 0x15, 0x40000000),
	AZALIA_PIN_CFG(1, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(1, 0x18, 0x01a19040),
	AZALIA_PIN_CFG(1, 0x19, 0x02a19050),
	AZALIA_PIN_CFG(1, 0x1a, 0x0181304f),
	AZALIA_PIN_CFG(1, 0x1b, 0x02214020),
	AZALIA_PIN_CFG(1, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(1, 0x1d, 0x40a4c601),
	AZALIA_PIN_CFG(1, 0x1e, AZALIA_PIN_CFG_NC(0)),
};

const u32 pc_beep_verbs[] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC662",
		.vendor_id    = 0x10ec0662,
		.subsystem_id = 0x18497662,
		.address      = 1,
		.verbs        = realtek_alc662_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc662_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
