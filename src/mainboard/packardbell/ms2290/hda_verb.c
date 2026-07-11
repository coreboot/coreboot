/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

static const u32 realtek_alc272x_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x10250379),
	AZALIA_PIN_CFG(0, 0x14, 0x99130110),
	AZALIA_PIN_CFG(0, 0x18, 0x03a11830),
	AZALIA_PIN_CFG(0, 0x19, 0x99a30920),
	AZALIA_PIN_CFG(0, 0x1d, 0x4017992d),
	AZALIA_PIN_CFG(0, 0x21, 0x0321101f),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x04, 0x18560010),
	AZALIA_PIN_CFG(3, 0x05, 0x58560020),
	AZALIA_PIN_CFG(3, 0x06, 0x58560030),
};

const u32 pc_beep_verbs[0] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC272X",
		.vendor_id    = 0x10ec0272,
		.subsystem_id = 0x10250379,
		.address      = 0,
		.verbs        = realtek_alc272x_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc272x_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862804,
		.subsystem_id = 0x80860101,
		.address      = 3,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
};

AZALIA_ARRAY_SIZES;
