/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <device/azalia_device.h>

static const u32 realtek_alc889_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x10ec0889),
	AZALIA_PIN_CFG(0, 0x11, 0x414471f0),
	AZALIA_PIN_CFG(0, 0x12, 0x59a3112e),
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, 0x01011014),
	AZALIA_PIN_CFG(0, 0x16, 0x01016011),
	AZALIA_PIN_CFG(0, 0x17, 0x01012012),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19030),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19020),
	AZALIA_PIN_CFG(0, 0x1a, 0x01813040),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214050),
	AZALIA_PIN_CFG(0, 0x1c, 0x9993114f),
	AZALIA_PIN_CFG(0, 0x1d, 0x59f00190),
	AZALIA_PIN_CFG(0, 0x1e, 0x014471f0),
	AZALIA_PIN_CFG(0, 0x1f, 0x41c451f0),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(3, 0x06, 0x18560010),
	AZALIA_PIN_CFG(3, 0x07, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC889",
		.vendor_id    = 0x10ec0889,
		.subsystem_id = 0x10ec0889,
		.address      = 0,
		.verbs        = realtek_alc889_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc889_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862805,
		.subsystem_id = 0x80860101,
		.address      = 3,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
