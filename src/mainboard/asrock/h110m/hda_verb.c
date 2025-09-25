/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

static const u32 realtek_alc887_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x10438445),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_OUT2, 0x40000000),
	AZALIA_PIN_CFG(0, ALC887_DMIC_LR, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_FRONT, 0x01014020),
	AZALIA_PIN_CFG(0, ALC887_SURROUND, 0x90170110),
	AZALIA_PIN_CFG(0, ALC887_CENTER_LFE, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_SIDE_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_MIC1, 0x01a19040),
	AZALIA_PIN_CFG(0, ALC887_MIC2, 0x02a19050),
	AZALIA_PIN_CFG(0, ALC887_LINE1, 0x0181304f),
	AZALIA_PIN_CFG(0, ALC887_LINE2, 0x02214030),
	AZALIA_PIN_CFG(0, ALC887_CD, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_PC_BEEP, 0x4026c629),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_OUT1, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_IN, AZALIA_PIN_CFG_NC(0)),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560020),
	AZALIA_PIN_CFG(2, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[] = {
};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC887",
		.vendor_id    = 0x10ec0887,
		.subsystem_id = 0x10438445,
		.address      = 0,
		.verbs        = realtek_alc887_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc887_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862809,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
