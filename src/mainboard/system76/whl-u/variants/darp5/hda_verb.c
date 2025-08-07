/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc293_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x15581325),
	AZALIA_RESET(1),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60140),
	AZALIA_PIN_CFG(0, 0x13, 0x40000000),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x15, 0x02211020),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x02a11050),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x41738205),
	AZALIA_PIN_CFG(0, 0x1e, 0x02451130),
};

static const u32 intel_hdmi_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x07, 0x18560010),
};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC293",
		.vendor_id    = 0x10ec0293,
		.subsystem_id = 0x15581325,
		.address      = 0,
		.verbs        = realtek_alc293_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc293_verbs),
	},
	{
		.name         = "Intel Kabylake HDMI",
		.vendor_id    = 0x8086280b,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_hdmi_verbs,
		.verb_count   = ARRAY_SIZE(intel_hdmi_verbs),
	},
	{ /* terminator */ }
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
