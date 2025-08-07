/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

static const u32 realtek_alc662_verbs_1[] = {
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19830),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19940),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181303f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214120),
	AZALIA_PIN_CFG(0, 0x1c, 0x593301f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x4004c601),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_PIN_CFG(1, 0x03, 0x18560010),
};

static const u32 realtek_alc662_verbs_2[] = {
	AZALIA_PIN_CFG(2, 0x14, 0x01014410),
	AZALIA_PIN_CFG(2, 0x15, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, 0x18, 0x01a19c30),
	AZALIA_PIN_CFG(2, 0x19, 0x02a19c40),
	AZALIA_PIN_CFG(2, 0x1a, 0x0181343f),
	AZALIA_PIN_CFG(2, 0x1b, 0x02214c20),
	AZALIA_PIN_CFG(2, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, 0x1d, 0x4004c601),
	AZALIA_PIN_CFG(2, 0x1e, AZALIA_PIN_CFG_NC(0)),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC662",
		.vendor_id    = 0x10ec0662,
		.subsystem_id = 0x18493662,
		.address      = 0,
		.verbs        = realtek_alc662_verbs_1,
		.verb_count   = ARRAY_SIZE(realtek_alc662_verbs_1),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862803,
		.subsystem_id = 0x80860101,
		.address      = 1,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
	{
		.name         = "Realtek ALC662",
		.vendor_id    = 0x10ec0662,
		.subsystem_id = 0x1565821e,
		.address      = 2,
		.verbs        = realtek_alc662_verbs_2,
		.verb_count   = ARRAY_SIZE(realtek_alc662_verbs_2),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
