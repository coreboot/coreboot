/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_HDA_VERB_H
#define MAINBOARD_HDA_VERB_H

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0257,	// Codec Vendor / Device ID: Realtek ALC257
	0x10ec0257,	// Subsystem ID
	0x0000000f,	// Number of jacks (NID entries)

	AZALIA_RESET(0x1),
	/* NID 0x01, HDA Codec Subsystem ID Verb table */
	AZALIA_SUBVENDOR(0, 0x10ec3a2c),

	/* Pin Widget Verb Table */

	/*
	 * DMIC
	 * Requirement is to use PCH DMIC. Hence,
	 * commented out codec's Internal DMIC.
	 * AZALIA_PIN_CFG(0, 0x12, 0x90A60130),
	 * AZALIA_PIN_CFG(0, 0x13, 0x40000000),
	 */

	/* Pin widget 0x14 - Front (Port-D) */
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	/* Pin widget 0x18 - NPC */
	AZALIA_PIN_CFG(0, 0x18, 0x411111F0),
	/* Pin widget 0x19 - MIC2 (Port-F) */
	AZALIA_PIN_CFG(0, 0x19, 0x04A11030),
	/* Pin widget 0x1A - LINE1 (Port-C) */
	AZALIA_PIN_CFG(0, 0x1a, 0x411111F0),
	/* Pin widget 0x1B - NPC */
	AZALIA_PIN_CFG(0, 0x1b, 0x411111F0),
	/* Pin widget 0x1D - BEEP-IN */
	AZALIA_PIN_CFG(0, 0x1d, 0x40579A2D),
	/* Pin widget 0x1E - NPC */
	AZALIA_PIN_CFG(0, 0x1e, 0x411111F0),
	/* Pin widget 0x21 - HP1-OUT (Port-I) */
	AZALIA_PIN_CFG(0, 0x21, 0x04211020),
	/* Widget node 0x20 - 0 */
	0x0205001A,
	0x0204C003,
	0x0205001A,
	0x0204C003,
	/* Widget node 0x20 - 1 */
	0x02050038,
	0x02046981,
	0x0205001B,
	0x02040A4B,
	/* Widget node 0x20 - 2 */
	0x0205003C,
	0x02043154,
	0x0205003C,
	0x02043114,
	/* Widget node 0x20 - 3 */
	0x02050046,
	0x02040004,
	0x05750003,
	0x057409A3,
	/* Widget node 0x20 - 4 */
	0x02050009,
	0x02044003,
	0x0205000A,
	0x02047778,
	/* Widget node 0x20 - 5 */
	0x02050037,
	0x0204FE15,
	0x02050030,
	0x02041000,
};

const u32 pc_beep_verbs[] = {
	/* Dos beep path - 1 */
	0x02050010,
	0x02040020,
	0x02050036,
	0x02047151,
	/* Dos beep path - 2 */
	0x01470740,
	0x0143B000,
	0x01470C02,
	0x01470C02,
};

AZALIA_ARRAY_SIZES;

#endif
