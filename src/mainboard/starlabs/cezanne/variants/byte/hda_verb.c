/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 conexant_cx20632_verbs[] = {
	/* Reset Codec First */
	AZALIA_RESET(0x1),

	/* HDA Codec Subsystem ID: 0x14f10216 */
	AZALIA_SUBVENDOR(0, 0x14f10216),

	/* Pin Widget Verb-table */
	AZALIA_PIN_CFG(0, 0x01, 0x00000000),
	/* Front headset jack */
	AZALIA_PIN_CFG(0, 0x19, 0x042b1010),
	/* Front mic jack */
	AZALIA_PIN_CFG(0, 0x1a, 0x04ab1020),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, AZALIA_PIN_CFG_NC(0)),
	/* Board-specific analog path; keep reference firmware pin config */
	AZALIA_PIN_CFG(0, 0x1e, 0x90a61120),
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x20, AZALIA_PIN_CFG_NC(0)),
	/* Board-specific analog path; keep reference firmware pin config */
	AZALIA_PIN_CFG(0, 0x21, 0x90171110),
	AZALIA_PIN_CFG(0, 0x26, AZALIA_PIN_CFG_NC(0)),

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
