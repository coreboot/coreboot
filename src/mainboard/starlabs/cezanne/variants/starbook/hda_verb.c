/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 conexant_cx20632_verbs[] = {
	/* Reset Codec First */
	AZALIA_RESET(0x1),

	/* HDA Codec Subsystem ID: 0x14f10216 */
	AZALIA_SUBVENDOR(0, 0x14f10216),

	/* Pin Widget Verb-table */
	AZALIA_PIN_CFG(0, 0x01, 0x00000000),
	AZALIA_PIN_CFG(0, 0x18,	AZALIA_PIN_DESC(
					AZALIA_INTEGRATED,
					AZALIA_INTERNAL | AZALIA_TOP,
					AZALIA_SPEAKER,
					AZALIA_OTHER_ANALOG,
					AZALIA_COLOR_UNKNOWN,
					AZALIA_NO_JACK_PRESENCE_DETECT,
					1,
					0
				)),
	AZALIA_PIN_CFG(0, 0x19,	AZALIA_PIN_DESC(
					AZALIA_JACK,
					AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_RIGHT,
					AZALIA_HP_OUT,
					AZALIA_STEREO_MONO_1_8,
					AZALIA_BLACK,
					AZALIA_JACK_PRESENCE_DETECT,
					4,
					0
				)),
	AZALIA_PIN_CFG(0, 0x1a,	AZALIA_PIN_DESC(
					AZALIA_JACK,
					AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_RIGHT,
					AZALIA_MIC_IN,
					AZALIA_STEREO_MONO_1_8,
					AZALIA_BLACK,
					AZALIA_JACK_PRESENCE_DETECT,
					2,
					0
				)),
	AZALIA_PIN_CFG(0, 0x1b,	AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1c,	AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d,	AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1e,	AZALIA_PIN_DESC(
					AZALIA_INTEGRATED,
					AZALIA_INTERNAL | AZALIA_TOP,
					AZALIA_MIC_IN,
					AZALIA_OTHER_DIGITAL,
					AZALIA_COLOR_UNKNOWN,
					AZALIA_NO_JACK_PRESENCE_DETECT,
					3,
					0
				)),
	AZALIA_PIN_CFG(0, 0x1f,	AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x20,	AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x21,	AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x26,	AZALIA_PIN_CFG_NC(0)),

	/* Enable EAPD */
	0x01870c02,
	0x01870c02,
	0x01870c02,
	0x01870c02,
};

const u32 pc_beep_verbs[] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Conexant CX20632",
		.vendor_id    = 0x14f15098,
		.subsystem_id = 0x14f10216,
		.address      = 0,
		.verbs        = conexant_cx20632_verbs,
		.verb_count   = ARRAY_SIZE(conexant_cx20632_verbs),
	},
};

AZALIA_ARRAY_SIZES;
