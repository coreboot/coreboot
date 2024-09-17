/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10134210,	// Codec Vendor / Device ID: Cirrus Logic CS4210
	0x152D0924,	// Subsystem ID
	0x00000008,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x152D0924 */
	AZALIA_SUBVENDOR(0, 0x152D0924),

	0x00170500,
	0x00170500,	/* Padding */
	0x00170500,	/* Padding */
	0x00170500,	/* Padding */

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x05) */
	AZALIA_PIN_CFG(0, 0x05, 0x022110f0),

	/* Pin Complex (NID 0x06) */
	AZALIA_PIN_CFG(0, 0x06, 0x901700f0),

	/* Pin Complex (NID 0x07) */
	AZALIA_PIN_CFG(0, 0x07, 0x02a110f0),

	/* Pin Complex (NID 0x08) */
	AZALIA_PIN_CFG(0, 0x08, 0x77a70037),

	/* Pin Complex (NID 0x09) */
	AZALIA_PIN_CFG(0, 0x09, 0xb7a6003e),

	/* Pin Complex (NID 0x0a) */
	AZALIA_PIN_CFG(0, 0x0a, 0x434510f0),
};

const u32 pc_beep_verbs[] = {
	0x00170500,			/* power up codec */
	0x00270500,			/* power up DAC */
	0x00670500,			/* power up speaker */
	0x00670740,			/* enable speaker output */
	0x0023B04B,			/* set DAC gain */
};
AZALIA_ARRAY_SIZES;
