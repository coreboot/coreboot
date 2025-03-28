/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0256,	/* Codec Vendor / Device ID: Realtek ALC256 */
	0x10ec12ac,	/* Subsystem ID */
	0x00000015,	/* Number of jacks (NID entries) */

	AZALIA_RESET(0x1),
	/* NID 0x01, HDA Codec Subsystem ID Verb Table */
	AZALIA_SUBVENDOR(0, 0x10ec12ac),

	/* Pin Widget Verb Table */
	/*
	 * DMIC
	 * Internal DMIC.
	 */
	AZALIA_PIN_CFG(0, 0x12, 0x90A60130),
	AZALIA_PIN_CFG(0, 0x13, 0x90A60140),
	/* Pin widget 0x14 - Front (Port-D) */
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	/* Pin widget 0x18 - NPC */
	AZALIA_PIN_CFG(0, 0x18, 0x411111F0),
	/* Pin widget 0x19 - MIC2 (Port-F) */
	AZALIA_PIN_CFG(0, 0x19, 0x04A11040),
	/* Pin widget 0x1A - LINE1 (Port-C) */
	AZALIA_PIN_CFG(0, 0x1a, 0x411111F0),
	/* Pin widget 0x1B - NPC */
	AZALIA_PIN_CFG(0, 0x1b, 0x411111F0),
	/* Pin widget 0x1D - BEEP-IN */
	AZALIA_PIN_CFG(0, 0x1d, 0x40610041),
	/* Pin widget 0x1E - NPC */
	AZALIA_PIN_CFG(0, 0x1e, 0x411111F0),
	/* Pin widget 0x21 - HP1-OUT (Port-I) */
	AZALIA_PIN_CFG(0, 0x21, 0x04211020),
	/*
	 * Widget node 0x20 - 1
	 * Codec hidden reset and speaker power 2W/4ohm
	 */
	0x0205001A,
	0x0204C003,
	0x02050038,
	0x02047901,
	/*
	 * Widget node 0x20 - 2
	 * Class D power on Reset
	 */
	0x0205003C,
	0x02040354,
	0x0205003C,
	0x02040314,
	/*
	 * Widget node 0x20 - 3
	 * Disable AGC and set AGC limit to -1.5dB
	 */
	0x02050016,
	0x02040C50,
	0x02050012,
	0x0204EBC1,
	/*
	 * Widget node 0x20 - 4
	 * Set AGC Post gain +1.5dB then Enable AGC
	 */
	0x02050013,
	0x02044023,
	0x02050016,
	0x02040E50,
	/*
	 * Widget node 0x20 - 5
	 * Silence detector enabling + Set EAPD to verb control
	 */
	0x02050037,
	0x0204FE15,
	0x02050010,
	0x02040020,
	/*
	 * Widget node 0x20 - 6
	 * Silence data mode Threshold (-90dB)
	 */
	0x02050030,
	0x0204A000,
	0x0205001B,
	0x02040A4B,
	/*
	 * Widget node 0x20 - 7
	 * Default setting-1
	 */
	0x05750003,
	0x05740DA3,
	0x02050046,
	0x02040004,
	/*
	 * Widget node 0x20 - 8
	 * support 1 pin detect two port
	 */
	0x02050009,
	0x0204E003,
	0x0205000A,
	0x02047770,
	/*
	 * Widget node 0x20 - 9
	 * To set LDO1/LDO2 as default
	 */
	0x02050008,
	0x02046A0C,
	0x02050008,
	0x02046A0C,
};

const u32 pc_beep_verbs[] = {
	/* Dos beep path - 1 */
	0x01470C00,
	0x02050036,
	0x02047151,
	0x01470740,
	/* Dos beep path - 2 */
	0x0143b000,
	0x01470C02,
	0x01470C02,
	0x01470C02,
};

AZALIA_ARRAY_SIZES;
