/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc1220_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x155865e5),
	AZALIA_RESET(1),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60130),
	AZALIA_PIN_CFG(0, 0x14, 0x0421101f),
	AZALIA_PIN_CFG(0, 0x15, 0x40000000),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x04a11040),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1b, 0x90170110),
	AZALIA_PIN_CFG(0, 0x1d, 0x40b7952d),
	AZALIA_PIN_CFG(0, 0x1e, 0x04451150),
};

const u32 pc_beep_verbs[] = {
	// Enable DMIC microphone on ALC1220
	0x02050036,
	0x02042a6a,
};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC1220",
		.vendor_id    = 0x10ec1220,
		.subsystem_id = 0x155865e5,
		.address      = 0,
		.verbs        = realtek_alc1220_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc1220_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
