/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

#if !CONFIG(AZALIA_USE_LEGACY_VERB_TABLE)

static const u32 display_audio_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x80860101),

	/* Port-B */
	AZALIA_PIN_CFG(2, 0x5, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_DIGITAL_DISPLAY,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		1, 0
	)),
	/* Port-C */
	AZALIA_PIN_CFG(2, 0x6, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_DIGITAL_DISPLAY,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		2, 0
	)),
	/* Port-D */
	AZALIA_PIN_CFG(2, 0x7, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_DIGITAL_DISPLAY,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		3, 0
	)),
};

static struct azalia_codec platform_codecs[] = {
	{
		.name         = "Intel Skylake Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862809,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = display_audio_verbs,
		.verb_count   = ARRAY_SIZE(display_audio_verbs),
	},
	{
		.name         = "Intel KabyLake Display Audio (HDMI/DP)",
		.vendor_id    = 0x8086280b,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = display_audio_verbs,
		.verb_count   = ARRAY_SIZE(display_audio_verbs),
	},
};

size_t azalia_get_platform_codecs(struct azalia_codec **codecs)
{
	*codecs = platform_codecs;
	return ARRAY_SIZE(platform_codecs);
}
#endif
