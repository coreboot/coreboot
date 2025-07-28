/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0257,	// Codec Vendor / Device ID: Realtek ALC257
	0x10ec0257,	// Subsystem ID
	0x00000011,	// Number of jacks (NID entries)

	AZALIA_RESET(0x1),
	/* NID 0x01, HDA Codec Subsystem ID Verb table */
	//AZALIA_SUBVENDOR(0, 0x10ec3a2c),

	/* Pin Widget Verb Table */
	/*
	 * DMIC
	 * Requirement is to use PCH DMIC. Hence,
	 * commented out codec's Internal DMIC.
	 * AZALIA_PIN_CFG(0, 0x12, 0x90A60130),
	 * AZALIA_PIN_CFG(0, 0x13, 0x40000000),
	 */
	 AZALIA_PIN_CFG(0, 0x12, 0x40000000),
	 AZALIA_PIN_CFG(0, 0x13, 0x411111F0),
	 AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	 AZALIA_PIN_CFG(0, 0x18, 0x411111f0),
	 AZALIA_PIN_CFG(0, 0x19, 0x04A11030),
	 AZALIA_PIN_CFG(0, 0x1a, 0x411111f0),
	 AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	 AZALIA_PIN_CFG(0, 0x1d, 0x40579A2D),
	 AZALIA_PIN_CFG(0, 0x1e, 0x411111F0),
	 AZALIA_PIN_CFG(0, 0x21, 0x04211020),
	//==========Widget node 0x20 - 0 :Hidden register SW reset
	0x0205001A,
	0x0204C003,
	0x0205001A,
	0x0204C003,
	//==========Widget node 0x20 - 1 : ClassD 2W
	0x02050038,
	0x02048981,
	0x0205001B,
	0x02040A4B,
	//==========Widget node 0x20 - 2
	0x0205003C,
	0x02043154,
	0x0205003C,
	0x02043114,
	//==========Widget node 0x20 - 3 :
	0x02050046,
	0x02040004,
	0x05750003,
	0x057409A3,
	//==========Widget node 0x20 - 4 :JD1 enable 1JD port for HP JD
	0x02050009,
	0x02046003,
	0x0205000A,
	0x02047770,
	//==========Widget node 0x20 - 5 : Silence data mode Threshold (-84dB)
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
