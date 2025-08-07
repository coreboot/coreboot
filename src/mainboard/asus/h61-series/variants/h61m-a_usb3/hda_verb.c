/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

static const u32 realtek_alc887_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x10438445),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_OUT2, 0x40330000),
	AZALIA_PIN_CFG(0, ALC887_DMIC_LR, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_FRONT, 0x01014010),
	AZALIA_PIN_CFG(0, ALC887_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_CENTER_LFE, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_SIDE_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_MIC1, 0x01a19030),
	AZALIA_PIN_CFG(0, ALC887_MIC2, 0x02a19040),
	AZALIA_PIN_CFG(0, ALC887_LINE1, 0x0181303f),
	AZALIA_PIN_CFG(0, ALC887_LINE2, 0x02214020),
	AZALIA_PIN_CFG(0, ALC887_CD, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_PC_BEEP, 0x4024c601),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_OUT1, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_IN, AZALIA_PIN_CFG_NC(0)),
};

static const u32 intel_display_audio_1_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x08, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0a, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0b, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0c, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0d, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0e, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0f, 0x18560010),
};

static const u32 intel_display_audio_2_verbs[] = {
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x58560010),
	AZALIA_PIN_CFG(3, 0x06, 0x58560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

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
		/*
		 * TODO: This seems out of place, the vendor ID suggests this
		 * codec is an Alder Lake P Intel Display Audio[1], and
		 * the user manual[2] does not mention this codec. Could someone
		 * with this board confirm what this is?
		 *
		 * [1] Linux kernel: sound/hda/codecs/hdmi/intelhdmi.c:789
		 * [2] H61M-A/USB3 User's Manual (English), Version E8184
		 */
		.name         = "Intel Display Audio #1 (HDMI/DP)",
		.vendor_id    = 0x8086281c,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_display_audio_1_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_1_verbs),
	},
	{
		.name         = "Intel Display Audio #2 (HDMI/DP)",
		.vendor_id    = 0x80862805,
		.subsystem_id = 0x80860101,
		.address      = 3,
		.verbs        = intel_display_audio_2_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_2_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
