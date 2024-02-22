/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <stdint.h>

const uint32_t cim_verb_data[] = {
	/* coreboot specific header */
	0x14f11f87,	/* Codec Vendor / Device ID: SoundWire SN6140 */
	0x14f1035e,	/* Subsystem ID */
	17,		/* Number of jacks (NID entries) */

	/* Reset Codec First */
	AZALIA_RESET(0x1),

	/* HDA Codec Subsystem ID Verb-table */
	AZALIA_SUBVENDOR(0, 0x14f1035e),

	/*
	 * Configuration:
	 *
	 * Port A (0x16)	04214040	Headphone
	 * Port B (0x18)	40F001F0
	 * Port C (0x1a)	90A72150	DMIC
	 * Port D (0x19)	04A19020	Headset Microphone
	 * Port E (0x1d)	0
	 * Port F (0x1e)	0
	 * Port G (0x17)	90173160	Speaker
	 * Port H (0x1f)	0
	 * Port I (0x21)	40f001f0
	 * Port M (0x22)	0
	 */

	0x01c49100,
	0x000f0000,
	0x000f0000,
	0x000f0000,

	0x000f0000,
	0x01c49000,
	0x000f0000,
	0x000f0000,

	0x0017ff00,
	0x0017ff00,
	0x000f0000,
	0x000f0000,

	0x000f0000,
	0x000f0000,
	0x000f0000,
	0x000f0000,

	AZALIA_PIN_CFG(0, 0x01, 0x00000000),

	0x0017205e,	/* SSID */
	0x00172103,
	0x001722f1,
	0x00172314,

	AZALIA_PIN_CFG(0, 0x16, 0x04214040),
	AZALIA_PIN_CFG(0, 0x18, 0x40f001f0),
	AZALIA_PIN_CFG(0, 0x1a,	0x90a72150),
	AZALIA_PIN_CFG(0, 0x19, 0x04a19020),
	AZALIA_PIN_CFG(0, 0x1d,	0x00000000),
	AZALIA_PIN_CFG(0, 0x1e, 0x00000000),
	AZALIA_PIN_CFG(0, 0x17, 0x90173160),
	AZALIA_PIN_CFG(0, 0x1f,	0x00000000),
	AZALIA_PIN_CFG(0, 0x21, 0x40f001f0),
	AZALIA_PIN_CFG(0, 0x22, 0x00000000),

	0x01c31028,	/* Class D Gain */
	0x01b30420,	/* Set EAPD */
	0x01c31028,
	0x01b30420,

	0x80862815,	/* Codec Vendor / Device ID: Intel */
	0x80860101,	/* Subsystem ID */
	10,		/* Number of 4 dword sets */

	AZALIA_SUBVENDOR(2, 0x80860101),

	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x08, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0a, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0b, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0c, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0d, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0e, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0f, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
