/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

static const u32 realtek_alc235_verbs[] = {
	/* Reset Codec First */
	AZALIA_RESET(0x1),

	/* HDA Codec Subsystem ID */
	AZALIA_SUBVENDOR(0, 0x20147017),

	/* Pin Widget Verb-table */
	AZALIA_PIN_CFG(0, ALC269_DMIC12,	AZALIA_PIN_DESC(
							AZALIA_INTEGRATED,
							AZALIA_MOBILE_LID_INSIDE,
							AZALIA_MIC_IN,
							AZALIA_OTHER_DIGITAL,
							AZALIA_COLOR_UNKNOWN,
							AZALIA_NO_JACK_PRESENCE_DETECT,
							3,
							0
						)),
	AZALIA_PIN_CFG(0, ALC269_MIC2,		AZALIA_PIN_DESC(
							AZALIA_JACK,
							AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_RIGHT,
							AZALIA_MIC_IN,
							AZALIA_STEREO_MONO_1_8,
							AZALIA_BLACK,
							AZALIA_JACK_PRESENCE_DETECT,
							4,
							0
						)),
	/* Internal speakers are connected to LINE2 */
	AZALIA_PIN_CFG(0, ALC269_LINE2,		AZALIA_PIN_DESC(
							AZALIA_INTEGRATED,
							AZALIA_INTERNAL | AZALIA_FRONT,
							AZALIA_SPEAKER,
							AZALIA_OTHER_ANALOG,
							AZALIA_COLOR_UNKNOWN,
							AZALIA_NO_JACK_PRESENCE_DETECT,
							1,
							0
						)),
	AZALIA_PIN_CFG(0, ALC269_VB_HP_OUT,	AZALIA_PIN_DESC(
							AZALIA_JACK,
							AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_RIGHT,
							AZALIA_HP_OUT,
							AZALIA_STEREO_MONO_1_8,
							AZALIA_BLACK,
							AZALIA_JACK_PRESENCE_DETECT,
							2,
							0
						)),
	AZALIA_PIN_CFG(0, ALC269_PC_BEEP,    AZALIA_PIN_DESC(
							AZALIA_NC,
							AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_GEOLOCATION_NA,
							AZALIA_LINE_IN,
							AZALIA_OTHER_DIGITAL,
							AZALIA_PINK,
							0xa,
							4,
							5
						)),
	AZALIA_PIN_CFG(0, 0x20,			AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC269_MONO,		AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC269_MIC1,		AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC269_SPEAKERS,	AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC269_LINE1,		AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC269_SPDIF_OUT,	AZALIA_PIN_CFG_NC(0)),

};

const u32 pc_beep_verbs[] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC235",
		.vendor_id    = 0x10ec0235,
		.subsystem_id = 0x20147017,
		.address      = 0,
		.verbs        = realtek_alc235_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc235_verbs),
	},
};

AZALIA_ARRAY_SIZES;
