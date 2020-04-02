/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x14f15051,	// Conexant CX20561 (Hermosa)
	0x17aa211c,	// Subsystem ID
	0x00000008,	// Number of entries

	/* Pin Widget Verb Table */

	AZALIA_PIN_CFG(0, 0x16, 0x022140f0),
	AZALIA_PIN_CFG(0, 0x17, 0x61a190f0),
	AZALIA_PIN_CFG(0, 0x18, 0x02a190f0),
	AZALIA_PIN_CFG(0, 0x19, 0x40f000f0),
	AZALIA_PIN_CFG(0, 0x1a, 0x901701f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x40f001f0),
	AZALIA_PIN_CFG(0, 0x1c, 0x40f001f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x90a601f0)
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
