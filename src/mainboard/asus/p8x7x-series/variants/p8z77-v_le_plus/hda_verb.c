/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <option.h>

/* Shorthands */
#define AZALIA_PIN_REAR(dev, color, misc, association, sequence) \
AZALIA_PIN_DESC(AZALIA_JACK, AZALIA_REAR, dev, AZALIA_STEREO_MONO_1_8, \
		color, misc, association, sequence)
#define AZALIA_PIN_FRONT(dev, color, misc, association, sequence) \
AZALIA_PIN_DESC(AZALIA_JACK, AZALIA_FRONT, dev, AZALIA_STEREO_MONO_1_8, \
		color, misc, association, sequence)

static const u32 realtek_alc889_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x1043841a),
	AZALIA_PIN_CFG(0, 0x11, AZALIA_PIN_DESC(
				AZALIA_INTEGRATED,
				AZALIA_ATAPI,
				AZALIA_SPDIF_OUT,
				AZALIA_ATAPI_INTERNAL,
				AZALIA_COLOR_UNKNOWN,
				AZALIA_NO_JACK_PRESENCE_DETECT,
				3, 0)), /* SPDIF out 2, 0x99430130 */
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x14, AZALIA_PIN_REAR(
				AZALIA_LINE_OUT,
				AZALIA_GREEN,
				0,
				1, 0)), /* 0x01014010 */
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_REAR(
				AZALIA_LINE_OUT,
				AZALIA_BLACK,
				0,
				1, 2)), /* 0x01011012 */
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_REAR(
				AZALIA_LINE_OUT,
				AZALIA_ORANGE,
				0,
				1, 1)), /* 0x01016011 */
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_REAR(
				AZALIA_LINE_OUT,
				AZALIA_GREY,
				0,
				1, 4)), /* 0x01012014 */
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_REAR(
				AZALIA_MIC_IN,
				AZALIA_PINK,
				8,
				5, 0)), /* 0x01a19850 */
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_FRONT(
				AZALIA_MIC_IN,
				AZALIA_PINK,
				0xc,
				6, 0)), /* 0x02a19c60 */
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_REAR(
				AZALIA_LINE_IN,
				AZALIA_BLUE,
				0,
				5, 15)), /* 0x0181305f */
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_FRONT(
				AZALIA_HP_OUT,
				AZALIA_GREEN,
				0xc,
				2, 0)), /* 0x02214c20),*/
	AZALIA_PIN_CFG(0, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x4005e601), /* Beep */
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_DESC(
				AZALIA_JACK,
				AZALIA_REAR,
				AZALIA_SPDIF_OUT,
				AZALIA_OPTICAL,
				AZALIA_ORANGE,
				AZALIA_NO_JACK_PRESENCE_DETECT,
				4, 0)), /* 0x01456140 */
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0)),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(3, 0x06, 0x58560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC889",
		.vendor_id    = 0x10ec0889,
		.subsystem_id = 0x1043841a,
		.address      = 0,
		.verbs        = realtek_alc889_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc889_verbs),
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

enum e_spdif2_dest {
	SPDIF2_SPDIF_OUT = 0,
	SPDIF2_HDMI
};

enum e_aafp {
	AAFP_HDA = 0,
	AAFP_AC97
};

void mainboard_azalia_program_runtime_verbs(u8 *base, u32 viddid)
{
	/*
	 * With vendor firmware, if audio front panel type is set as AC97, line out 2
	 * (0x1b) and mic 2 (0x19) pins of ALC889 are configured differently.
	 *
	 * The differences are all in the "Misc" fields of configuration defaults (in byte 2)
	 * as shown below. Datasheet did not offer details on what those bits
	 * (listed as reserved in HDA spec) are, so we'll have to take their word for it.
	 *
	 * Pin  | 0x19 | 0x1b
	 * -----+------+-----
	 * HDA  | 1100 | 1100
	 * AC97 | 1001 | 0001
	 */
	const u32 front_aafp_verbs[] = {
		AZALIA_VERB_12B(0, 0x19, 0x71d, 0x99),
		AZALIA_VERB_12B(0, 0x1b, 0x71d, 0x41)
	};

	/* If user chooses to have digital audio out 2 to go on the HDMI port,
	 * reprogram this pin as such:
	 *
	 *	AZALIA_PIN_CFG(0, 0x11, AZALIA_PIN_DESC(
	 *				AZALIA_JACK,
	 *				AZALIA_DIGITAL_DISPLAY,
	 *				AZALIA_DIGITAL_OTHER_OUT,
	 *				AZALIA_OTHER_DIGITAL,
	 *				AZALIA_BLACK,
	 *				AZALIA_NO_JACK_PRESENCE_DETECT,
	 *				3, 0))
	 *
	 * Coded this way because we don't need to reprogram byte 0.
	 */
	const u32 spdif_to_hdmi_verbs[] = {
		AZALIA_VERB_12B(0, 0x11, 0x71f, 0x18),
		AZALIA_VERB_12B(0, 0x11, 0x71e, 0x56),
		AZALIA_VERB_12B(0, 0x11, 0x71d, 0x11)
	};

	if (viddid == 0x10ec0889) {
		if (get_uint_option("audio_panel_type", AAFP_HDA) == AAFP_AC97) {
			azalia_program_verb_table(base, front_aafp_verbs,
			ARRAY_SIZE(front_aafp_verbs));
		}
		if (get_uint_option("spdif_dest", SPDIF2_SPDIF_OUT) == SPDIF2_HDMI) {
			azalia_program_verb_table(base, spdif_to_hdmi_verbs,
			ARRAY_SIZE(spdif_to_hdmi_verbs));
		}
	}
}
