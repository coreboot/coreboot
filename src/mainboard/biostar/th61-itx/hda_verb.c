/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc892_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x15658229),
	AZALIA_PIN_CFG(2, 0x11, 0x01452130),
	AZALIA_PIN_CFG(2, 0x12, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, 0x14, 0x01014410),
	AZALIA_PIN_CFG(2, 0x15, 0x01011412),
	AZALIA_PIN_CFG(2, 0x16, 0x01016411),
	AZALIA_PIN_CFG(2, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, 0x18, 0x01a19c40),
	AZALIA_PIN_CFG(2, 0x19, 0x02a19850),
	AZALIA_PIN_CFG(2, 0x1a, 0x0181344f),
	AZALIA_PIN_CFG(2, 0x1b, 0x02214020),
	AZALIA_PIN_CFG(2, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, 0x1d, 0x4005e601),
	AZALIA_PIN_CFG(2, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, 0x1f, AZALIA_PIN_CFG_NC(0)),
};

const u32 pc_beep_verbs[0] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC892",
		.vendor_id    = 0x10ec0892,
		.subsystem_id = 0x15658229,
		.address      = 2,
		.verbs        = realtek_alc892_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc892_verbs),
	},
};

AZALIA_ARRAY_SIZES;
