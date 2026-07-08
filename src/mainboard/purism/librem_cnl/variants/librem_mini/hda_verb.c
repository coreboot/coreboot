/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc269_verbs[] = {
	AZALIA_RESET(0x1),

	AZALIA_SUBVENDOR(0, 0x10ec0269),
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)), /* NC */
	AZALIA_PIN_CFG(0, 0x14, AZALIA_PIN_CFG_NC(0)), /* NC */
	AZALIA_PIN_CFG(0, 0x15, 0x02211010), /* Jack analog out */
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)), /* NC */
	AZALIA_PIN_CFG(0, 0x18, 0x02a11120), /* Jack analog mic, no presence detect */
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)), /* NC */
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)), /* NC */
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)), /* NC */
	AZALIA_PIN_CFG(0, 0x1d, AZALIA_PIN_CFG_NC(0)), /* NC */
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)), /* NC */
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x07, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC269",
		.vendor_id    = 0x10ec0269,
		.subsystem_id = 0x10ec0269,
		.address      = 0,
		.verbs        = realtek_alc269_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc269_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x8086280b,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
};

AZALIA_ARRAY_SIZES;
