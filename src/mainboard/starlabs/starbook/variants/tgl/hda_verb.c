/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

static const u32 realtek_alc256_verbs[] = {
#include <mainboard/starlabs/common/hda/alc256_internal.inc>
};

static const u32 intel_display_audio_codecs[] = {
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC256",
		.vendor_id    = 0x10ec0256,
		.subsystem_id = 0x10ec1200,
		.address      = 0,
		.verbs        = realtek_alc256_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc256_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862812,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_display_audio_codecs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_codecs),
	},
};

AZALIA_ARRAY_SIZES;
