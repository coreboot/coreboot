/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0236,	// Codec Vendor / Device ID: Realtek ALC236
	0x103C8C60,	// Subsystem ID
	0x00000017,	// Number of jacks (NID entries)

	AZALIA_RESET(0x1),
	/* NID 0x01, HDA Codec Subsystem ID Verb table */
	AZALIA_SUBVENDOR(0, 0x103C8C60),

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
	AZALIA_PIN_CFG(0, 0x19, 0x03A11020),
	/* Pin widget 0x1A - LINE1 (Port-C) */
	AZALIA_PIN_CFG(0, 0x1a, 0x411111F0),
	/* Pin widget 0x1B - NPC */
	AZALIA_PIN_CFG(0, 0x1b, 0x411111F0),
	/* Pin widget 0x1D - BEEP-IN */
	AZALIA_PIN_CFG(0, 0x1d, 0x40600001),
	/* Pin widget 0x1E - NPC */
	AZALIA_PIN_CFG(0, 0x1e, 0x411111F0),
	/* Pin widget 0x21 - HP1-OUT (Port-I) */
	AZALIA_PIN_CFG(0, 0x21, 0x03211040),

	/*
	 * Widget node 0x20 - 1
	 */
	0x0205003C,
	0x02040354,
	0x0205003C,
	0x02040314,
	/*
	 * Widget node 0x20 - 2
	 * Set JD2 pull high
	 */
	0x0205001B,
	0x02040A4B,
	0x0205000B,
	0x02047778,
	/*
	 * Widget node 0x20 - 3
	 */
	0x02050046,
	0x02040004,
	0x05750003,
	0x057409A3,
	/* disable EQ first */
	0x05350000,
	0x0534201A,
	/* Left Channel */
	0x0535001d,
	0x05340800,
	0x0535001e,
	0x05340800,
	0x05350003,
	0x05341F7B,
	0x05350004,
	0x05340000,
	/* Right Channel */
	0x05450000,
	0x05442000,
	0x0545001d,
	0x05440800,
	0x0545001e,
	0x05440800,
	0x05450003,
	0x05441F7B,
	0x05450004,
	0x05440000,
	/* enable EQ */
	0x05350000,
	0x0534E01A,
	/* 1.8W/4ohm */
	0x02050038,
	0x02047901,
	/* AGC Enable */
	0x0205004C,
	0x0204465C,
	0x02050016,
	0x02044E50,
	0x02050020,
	0x020451FF,
	/* Headphone Pop */
	0x05750007,
	0x057412B2,
};

const u32 pc_beep_verbs[] = {
	/* Dos beep path - 1 */
	0x02050036,
	0x02047151,
	0x02050010,
	0x02040020,
	/* Dos beep path - 2 */
	0x0143B000,
	0x01470740,
	0x01470C02,
	0x01470C02,
};

AZALIA_ARRAY_SIZES;
