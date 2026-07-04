/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

#if !CONFIG(AZALIA_USE_LEGACY_VERB_TABLE)

static const u32 display_audio_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x80860101),

	/* Port 1 */
	AZALIA_PIN_CFG(2, 0x4, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_DIGITAL_DISPLAY,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		1, 0
	)),
	/* Port 2 */
	AZALIA_PIN_CFG(2, 0x6, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_DIGITAL_DISPLAY,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		2, 0
	)),
	/* Port 3 */
	AZALIA_PIN_CFG(2, 0x8, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_DIGITAL_DISPLAY,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		3, 0
	)),
	/* Port 4 */
	AZALIA_PIN_CFG(2, 0xa, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_DIGITAL_DISPLAY,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		4, 0
	)),
	/* Port 5 */
	AZALIA_PIN_CFG(2, 0xb, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_DIGITAL_DISPLAY,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		5, 0
	)),
	/* Port 6 */
	AZALIA_PIN_CFG(2, 0xc, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_DIGITAL_DISPLAY,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		6, 0
	)),
	/* Port 7 */
	AZALIA_PIN_CFG(2, 0xd, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_DIGITAL_DISPLAY,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		7, 0
	)),
	/* Port 8 */
	AZALIA_PIN_CFG(2, 0xe, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_DIGITAL_DISPLAY,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		8, 0
	)),
	/* Port 9 */
	AZALIA_PIN_CFG(2, 0xf, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_DIGITAL_DISPLAY,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		9, 0
	)),
};

static struct azalia_codec platform_codecs[] = {
	{
		.name         = "Intel AlderLake Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862815,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = display_audio_verbs,
		.verb_count   = ARRAY_SIZE(display_audio_verbs),
	},
	{
		.name         = "Intel RaptorLake Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862818,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = display_audio_verbs,
		.verb_count   = ARRAY_SIZE(display_audio_verbs),
	},
	{
		.name         = "Intel AlderLake-P Display Audio (HDMI/DP)",
		.vendor_id    = 0x8086281c,
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
