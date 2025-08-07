/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

static const u32 realtek_alc1200_verbs[] = {
	AZALIA_PIN_CFG(0, 0x11, 0x99430130),
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, 0x01011012),
	AZALIA_PIN_CFG(0, 0x16, 0x01016011),
	AZALIA_PIN_CFG(0, 0x17, 0x01012014),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19840),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19c50),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181304f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214c20),
	AZALIA_PIN_CFG(0, 0x1c, 0x593301f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x4015e601),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0)),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_PIN_CFG(1, 0x03, 0x18560010),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC1200",
		.vendor_id    = 0x10ec0888,
		.subsystem_id = 0x104382fe,
		.address      = 0,
		.verbs        = realtek_alc1200_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc1200_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862803,
		.subsystem_id = 0x80860101,
		.address      = 1,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
