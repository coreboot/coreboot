/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

static const u32 realtek_alc892_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x1849c892),
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x14, 0x01014020),
	AZALIA_PIN_CFG(0, 0x17, 0x90170110),
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, 0x02a11c3f),
	AZALIA_PIN_CFG(0, 0x1b, 0x01813c30),
	AZALIA_PIN_CFG(0, 0x1d, 0x598301f0),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x21, 0x0221102f),
};

static const u32 realtek_alc887_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x1458a002),
	AZALIA_PIN_CFG(2, ALC887_SPDIF_OUT2, 0x411110f0),
	AZALIA_PIN_CFG(2, ALC887_DMIC_LR, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, ALC887_FRONT, 0x01014410),
	AZALIA_PIN_CFG(2, ALC887_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, ALC887_CENTER_LFE, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, ALC887_SIDE_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, ALC887_MIC1, 0x01a19c50),
	AZALIA_PIN_CFG(2, ALC887_MIC2, 0x02a19c60),
	AZALIA_PIN_CFG(2, ALC887_LINE1, 0x0181345f),
	AZALIA_PIN_CFG(2, ALC887_LINE2, 0x02214c20),
	AZALIA_PIN_CFG(2, ALC887_CD, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, ALC887_PC_BEEP, 0x4004c601),
	AZALIA_PIN_CFG(2, ALC887_SPDIF_OUT1, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, ALC887_SPDIF_IN, 0x41c46060),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x58560010),
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),
	AZALIA_PIN_CFG(3, 0x07, 0x58560030),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC892",
		.vendor_id    = 0x10ec0892,
		.subsystem_id = 0x1849c892,
		.address      = 0,
		.verbs        = realtek_alc892_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc892_verbs),
	},
	{
		.name         = "Realtek ALC887",
		.vendor_id    = 0x10ec0887,
		.subsystem_id = 0x1458a002,
		.address      = 2,
		.verbs        = realtek_alc887_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc887_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862806,
		.subsystem_id = 0x80860101,
		.address      = 3,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
