/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc897_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x10ec0897),
	AZALIA_PIN_CFG(0, 0x11, 0x40000000),
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x14, 0x01214010),
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19030),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x40210201),
	AZALIA_PIN_CFG(0, 0x1e, 0x01441120),
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0)),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_VERB_12B(2, 0x02, 0x781, 3),
	AZALIA_VERB_12B(2, 0x00, 0xf00, 0),
	AZALIA_VERB_12B(2, 0x00, 0xf00, 0),
	AZALIA_VERB_12B(2, 0x00, 0xf00, 0),
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

const u32 pc_beep_verbs[] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC897",
		.vendor_id    = 0x10ec0897,
		.subsystem_id = 0x10ec0897,
		.address      = 0,
		.verbs        = realtek_alc897_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc897_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x8086281c,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
};

AZALIA_ARRAY_SIZES;
