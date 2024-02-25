/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0255,	/* Codec Vendor / Device ID: Realtek ALC255 */
	0xffffffff,	/* Subsystem ID */
	12,		/* Number of entries below */

	/* Reset Codec First */
	AZALIA_RESET(0x1),
	/* NID 0x01, HDA Codec Subsystem ID Verb Table */
	AZALIA_SUBVENDOR(0, 0x110a4097),

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(0, 0x14, AZALIA_PIN_DESC( /* 0x14 Speaker OUT */
		AZALIA_INTEGRATED,
		AZALIA_INTERNAL,
		AZALIA_SPEAKER,
		AZALIA_OTHER_ANALOG,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_NO_JACK_PRESENCE_DETECT,
		1, 0
	)),
	AZALIA_PIN_CFG(0, 0x21, AZALIA_PIN_DESC( /* 0x21 Headphone OUT */
		AZALIA_JACK,
		AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_FRONT,
		AZALIA_HP_OUT,
		AZALIA_COMBINATION,
		AZALIA_BLACK,
		AZALIA_JACK_PRESENCE_DETECT,
		2, 0
	)),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_DESC( /* 0x19 MIC2 */
		AZALIA_JACK,
		AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_FRONT,
		AZALIA_MIC_IN,
		AZALIA_COMBINATION,
		AZALIA_BLACK,
		AZALIA_NO_JACK_PRESENCE_DETECT,
		3, 0
	)),

	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)),	/* 0x12 Digital MIC */
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(1)),	/* 0x17 Mono OUT */
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(2)),	/* 0x18 MIC1 */
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(3)),	/* 0x1a LINE1 */
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(4)),	/* 0x1b LINE2 */
	AZALIA_PIN_CFG(0, 0x1d, AZALIA_PIN_CFG_NC(5)),	/* 0x1d PCBEEP */
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(6)),	/* 0x1e S/PDIF OUT */

	/* HDMI/DP audio codec */
	0x8086280b,	/* Codec Vendor / Device ID: Intel Kabylake HDMI */
	0xffffffff,	/* Subsystem ID */
	5,		/* Number of entries below */

	/* Enable 2nd & 3rd pin widgets first */
	0x20878101,
	0x20878101,
	0x20878101,
	0x20878101,
	AZALIA_PIN_CFG(2, 0x05, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, 0x06, AZALIA_PIN_CFG_NC(1)),
	AZALIA_PIN_CFG(2, 0x07, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_REAR_PANEL,
		AZALIA_DIGITAL_OTHER_OUT,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_JACK_PRESENCE_DETECT,
		1, 0
	)),
	/* Disable 2nd & 3rd pin widgets again */
	0x20878100,
	0x20878100,
	0x20878100,
	0x20878100,
};

const u32 pc_beep_verbs[] = {
};

AZALIA_ARRAY_SIZES;
