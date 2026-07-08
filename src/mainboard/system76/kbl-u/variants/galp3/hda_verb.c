/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc269vc_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x15581313),
	AZALIA_RESET(1),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60140),
	AZALIA_PIN_CFG(0, 0x14, 0x90170120),
	AZALIA_PIN_CFG(0, 0x15, 0x02211010),
	AZALIA_PIN_CFG(0, 0x17, 0x40000000),
	AZALIA_PIN_CFG(0, 0x18, 0x02a11030),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x40f4a205),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
};

static const u32 intel_kabylake_hdmi_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x07, 0x18560010),
};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC269",
		.vendor_id    = 0x10ec0269,
		.subsystem_id = 0x15581313,
		.address      = 0,
		.verbs        = realtek_alc269vc_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc269vc_verbs),
	},
	{
		.name         = "Intel Kabylake HDMI",
		.vendor_id    = 0x8086280b,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_kabylake_hdmi_verbs,
		.verb_count   = ARRAY_SIZE(intel_kabylake_hdmi_verbs),
	},
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
