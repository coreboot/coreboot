/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 cirrus_logic_cs4206_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x106b5b00),
	AZALIA_PIN_CFG(0, 0x09, 0x012b4030),
	AZALIA_PIN_CFG(0, 0x0a, 0x400000f0),
	AZALIA_PIN_CFG(0, 0x0b, 0x90100120),
	AZALIA_PIN_CFG(0, 0x0c, 0x400000f0),
	AZALIA_PIN_CFG(0, 0x0d, 0x90a00110),
	AZALIA_PIN_CFG(0, 0x0e, 0x400000f0),
	AZALIA_PIN_CFG(0, 0x0f, 0x400000f0),
	AZALIA_PIN_CFG(0, 0x10, 0x400000f0),
	AZALIA_PIN_CFG(0, 0x12, 0x400000f0),
	AZALIA_PIN_CFG(0, 0x15, 0x400000f0),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(3, 0x06, 0x18560010),
	AZALIA_PIN_CFG(3, 0x07, 0x18560010),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Cirrus Logic CS4206",
		.vendor_id    = 0x10134206,
		.subsystem_id = 0x106b5b00,
		.address      = 0,
		.verbs        = cirrus_logic_cs4206_verbs,
		.verb_count   = ARRAY_SIZE(cirrus_logic_cs4206_verbs),
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
