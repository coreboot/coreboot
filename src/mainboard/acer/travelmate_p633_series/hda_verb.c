/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 conexant_cx20588_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x10250737),
	AZALIA_PIN_CFG(0, 0x19, 0x03211040),
	AZALIA_PIN_CFG(0, 0x1a, 0x400001f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x03a19020),
	AZALIA_PIN_CFG(0, 0x1c, 0x400001f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x61811030),
	AZALIA_PIN_CFG(0, 0x1e, 0x400001f0),
	AZALIA_PIN_CFG(0, 0x1f, 0x92170110),
	AZALIA_PIN_CFG(0, 0x20, 0x400001f0),
	AZALIA_PIN_CFG(0, 0x22, 0x400001f0),
	AZALIA_PIN_CFG(0, 0x23, 0x90a6012e),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),
	AZALIA_PIN_CFG(3, 0x07, 0x58560030),
};

const u32 pc_beep_verbs[] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name		= "Conexant CX20588",
		.vendor_id	= 0x14f1506c,
		.subsystem_id	= 0x10250737,
		.address	= 0,
		.verbs		= conexant_cx20588_verbs,
		.verb_count	= ARRAY_SIZE(conexant_cx20588_verbs),
	},
	{
		.name		= "Intel PantherPoint HDMI",
		.vendor_id	= 0x80862806,
		.subsystem_id	= 0x80860101,
		.address	= 3,
		.verbs		= intel_display_audio_verbs,
		.verb_count	= ARRAY_SIZE(intel_display_audio_verbs),
	},
};

AZALIA_ARRAY_SIZES;
