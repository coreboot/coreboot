/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10134210,	// Codec Vendor / Device ID: Cirrus Logic CS4210
	0x152D0924,	// Subsystem ID
	0x00000008,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x152D0924 */
	AZALIA_SUBVENDOR(0, 0x152D0924),

	/* Padding - Power up sequence */
	0x00170500,
	0x00170500,
	0x00170500,
	0x00170500,

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x05) - Headphone Jack */
	AZALIA_PIN_CFG(0, 0x05, AZALIA_PIN_DESC(
						AZALIA_JACK,
						AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_FRONT,
						AZALIA_HP_OUT,
						AZALIA_STEREO_MONO_1_8,
						AZALIA_BLACK,
						AZALIA_JACK_PRESENCE_DETECT,
						15,
						0
					)),

	/* Pin Complex (NID 0x06) - Internal Speaker */
	AZALIA_PIN_CFG(0, 0x06, AZALIA_PIN_DESC(
						AZALIA_INTEGRATED,
						AZALIA_INTERNAL,
						AZALIA_SPEAKER,
						AZALIA_OTHER_ANALOG,
						AZALIA_COLOR_UNKNOWN,
						AZALIA_JACK_PRESENCE_DETECT,
						15,
						0
					)),

	/* Pin Complex (NID 0x07) - Microphone Jack */
	AZALIA_PIN_CFG(0, 0x07, AZALIA_PIN_DESC(
						AZALIA_JACK,
						AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_FRONT,
						AZALIA_MIC_IN,
						AZALIA_STEREO_MONO_1_8,
						AZALIA_BLACK,
						AZALIA_JACK_PRESENCE_DETECT,
						15,
						0
					)),

	/* Pin Complex (NID 0x08) - Unused (NC) */
	AZALIA_PIN_CFG(0, 0x08, AZALIA_PIN_DESC(
					AZALIA_NC,
					AZALIA_MOBILE_LID_INSIDE,
					AZALIA_MIC_IN,
					AZALIA_OTHER_ANALOG,
					AZALIA_COLOR_UNKNOWN,
					AZALIA_JACK_PRESENCE_DETECT,
					3,
					7
				)),

	/* Pin Complex (NID 0x09) - Internal Digital Mic */
	AZALIA_PIN_CFG(0, 0x09, AZALIA_PIN_DESC(
					AZALIA_INTEGRATED,
					AZALIA_MOBILE_LID_INSIDE,
					AZALIA_MIC_IN,
					AZALIA_OTHER_DIGITAL,
					AZALIA_COLOR_UNKNOWN,
					AZALIA_JACK_PRESENCE_DETECT,
					3,
					14
				)),

	/* Pin Complex (NID 0x0a) - SPDIF Out (NC) */
	AZALIA_PIN_CFG(0, 0x0a, AZALIA_PIN_DESC(
					AZALIA_NC,
					AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_LEFT,
					AZALIA_SPDIF_OUT,
					AZALIA_OPTICAL,
					AZALIA_BLACK,
					AZALIA_JACK_PRESENCE_DETECT,
					15,
					0
				)),
};

const u32 pc_beep_verbs[] = {
	0x00170500,			/* power up codec */
	0x00270500,			/* power up DAC */
	0x00670500,			/* power up speaker */
	0x00670740,			/* enable speaker output */
	0x0023B04B,			/* set DAC gain */
};
AZALIA_ARRAY_SIZES;
