/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

/*
 * Derived from Realtek HD Audio Configuration (ALC256),
 * HDAUDIO\FUNC_01&VEN_10EC&DEV_0256&SUBSYS_103C0000.
 */

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0256,	/* Codec Vendor / Device ID: Realtek ALC256 */
	0x103c0000,	/* Subsystem ID */
	0x00000022,	/* Number of jacks (groups of 4 dwords) after this header */

	AZALIA_RESET(0x1),

	/* Codec hidden reset (power on default) */
	0x0205001a,
	0x0204c003,
	0x0205001a,
	0x02048003,
	0x0205007f,
	0x02046001,
	0x0205007f,
	0x02040001,

	/* NID 0x01, HDA Codec Subsystem ID Verb Table */
	AZALIA_SUBVENDOR(0, 0x103c0000),

	/* Pin Widget Verb Table */
	/* Pin widget 0x12 - DMIC */
	AZALIA_PIN_CFG(0, 0x12, 0x40000000),
	/* Pin widget 0x13 - DMIC */
	AZALIA_PIN_CFG(0, 0x13, 0x411111f0),
	/* Pin widget 0x14 - Front (Port-D) */
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	/* Pin widget 0x18 - NPC */
	AZALIA_PIN_CFG(0, 0x18, 0x411111f0),
	/* Pin widget 0x19 - MIC2 (Port-F) */
	AZALIA_PIN_CFG(0, 0x19, 0x03a11030),
	/* Pin widget 0x1a - LINE1 (Port-C) */
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0),
	/* Pin widget 0x1b - LINE2 (Port-E) */
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	/* Pin widget 0x1d - BEEP-IN */
	AZALIA_PIN_CFG(0, 0x1d, 0x40400001),
	/* Pin widget 0x1e - S/PDIF-OUT */
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
	/* Pin widget 0x21 - HP1-OUT (Port-I) */
	AZALIA_PIN_CFG(0, 0x21, 0x03211020),

	/* Widget node 0x20 - 1 */
	0x0205003c,
	0x02040354,
	0x0205003c,
	0x02040314,
	/*
	 * Widget node 0x20 - 2
	 * set JD2 pull high
	 */
	0x0205001b,
	0x02040a4b,
	0x0205000b,
	0x02047778,
	/* Widget node 0x20 - 3 */
	0x02050046,
	0x02040004,
	0x05750003,
	0x057409a3,
	/* EQ verb table: disable EQ first */
	0x05350000,
	0x0534202a,
	/* Left channel */
	0x0535001d,
	0x05340800,
	0x0535001e,
	0x05340800,
	0x05350005,
	0x053403f6,
	0x05350006,
	0x0534854c,
	0x05350007,
	0x05341e09,
	0x05350008,
	0x05346472,
	0x05350009,
	0x053401fb,
	0x0535000a,
	0x05344836,
	0x0535000b,
	0x05341c00,
	0x0535000c,
	0x05340000,
	0x0535000d,
	0x05340200,
	0x0535000e,
	0x05340000,
	/* Right channel */
	0x05450000,
	0x05442000,
	0x0545001d,
	0x05440800,
	0x0545001e,
	0x05440800,
	0x05450005,
	0x054403f6,
	0x05450006,
	0x0544854c,
	0x05450007,
	0x05441e09,
	0x05450008,
	0x05446472,
	0x05450009,
	0x054401fb,
	0x0545000a,
	0x05444836,
	0x0545000b,
	0x05441c00,
	0x0545000c,
	0x05440000,
	0x0545000d,
	0x05440200,
	0x0545000e,
	0x05440000,
	/* Enable EQ */
	0x05350000,
	0x0534e02a,
	/* Widget node 0x20 - vendor registers */
	0x02050038,
	0x02046901,
	0x02050013,
	0x0204401f,
	0x02050016,
	0x02044c50,
	0x02050012,
	0x0204ebc0,
	0x02050020,
	0x020451ff,
	0x02050016,
	0x02044e50,
	/* Headphone Pop */
	0x05750007,
	0x057412b2,
	0x05750007,
	0x057412b2,
};

const u32 pc_beep_verbs[] = {
	/* PC-BEEP Enable in DOS */
	0x02050036,
	0x02047151,
	0x02050000,
	0x02050010,
	0x02040020,
	0x02050000,
	/* PC-BEEP Enable in DOS - 2 */
	0x00170500,
	0x0143b000,
	0x01470740,
	0x01470c00,
};

AZALIA_ARRAY_SIZES;
