/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0262,	/* Codec Vendor / Device ID: Realtek ALC262 */
	0x43528986,	/* Subsystem ID */
	0x0000000c,	/* Number of entries */

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x11), S/PDIF-OUT2: not connected */
	AZALIA_PIN_CFG(0, 0x11, AZALIA_PIN_CFG_NC(0)),
	/* Pin Complex (NID 0x14), LINE_OUT (port D): Speakers */
	AZALIA_PIN_CFG(0, 0x14, 0x99130110),
	/* Pin Complex (NID 0x15), HP_OUT (port A): Head phones */
	AZALIA_PIN_CFG(0, 0x15, 0x0121411f),
	/* Pin Complex (NID 0x16), MONO-OUT: not connected */
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	/* Pin Complex (NID 0x18), MIC1 (port B): Microphone */
	AZALIA_PIN_CFG(0, 0x18, 0x01a19920),
	/* Pin Complex (NID 0x19), MIC2 (port F): not connected */
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	/* Pin Complex (NID 0x1a), LINE1 (port C): Line-In */
	AZALIA_PIN_CFG(0, 0x1a, 0x01813121),
	/* Pin Complex (NID 0x1b), LINE2 (port E): MDC */
	AZALIA_PIN_CFG(0, 0x1b, 0x9983012f),
	/* Pin Complex (NID 0x1c), CD_IN */
	AZALIA_PIN_CFG(0, 0x1c, 0x593301f0),
	/* Pin Complex (NID 0x1d), PCBEEP */
	AZALIA_PIN_CFG(0, 0x1d, 0x4014022d),
	/* Pin Complex (NID 0x1e), S/PDIF-OUT: not connected */
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	/* Pin Complex (NID 0x1f), S/PDIF-IN: not connected */
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0))
};

const u32 pc_beep_verbs[] = {
	0x00170500,	/* power up codec */
	0x01470500,	/* power up speakers */
	0x01470100,	/* select lout1 (input 0x0) for speakers */
	0x01470740,	/* enable speakers output */
	0x00b37517,	/* unmute beep (mixer's input 0x5), set amp 0dB */
	0x00c37100,	/* unmute mixer in lout1 (lout1 input 0x1) */
	0x00c3b015,	/* set lout1 output volume -15dB */
	0x0143b000,	/* unmute speakers */
};
AZALIA_ARRAY_SIZES;
