/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef GA_B75_D3V_HDA_VERB_H
#define GA_B75_D3V_HDA_VERB_H

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0887,	// Realtek 887
	0x1458a002,	// Subsystem ID
	0x0000000e,	// Number of entries

	/* NID 0x01: Subsystem ID.  */
	AZALIA_SUBVENDOR(2, 0x1458a002),

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(2, 0x11, 0x99430130),
	AZALIA_PIN_CFG(2, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x14, 0x01014410),
	AZALIA_PIN_CFG(2, 0x15, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x16, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x18, 0x01a19c50),
	AZALIA_PIN_CFG(2, 0x19, 0x02a19c60),
	AZALIA_PIN_CFG(2, 0x1a, 0x0181345f),
	AZALIA_PIN_CFG(2, 0x1b, 0x02214c20),
	AZALIA_PIN_CFG(2, 0x1c, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x1d, 0x4004c601),
	AZALIA_PIN_CFG(2, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x1f, 0x411111f0),
};

#endif
