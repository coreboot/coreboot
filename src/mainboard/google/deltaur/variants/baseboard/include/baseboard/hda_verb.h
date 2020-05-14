/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_HDA_VERB_H
#define MAINBOARD_HDA_VERB_H

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0236,	// Codec Vendor / Device ID: Realtek ALC3204
	0xffffffff,	// Subsystem ID
	0x0000002b,	// Number of jacks (NID entries)
	/* Rest Codec First */
	AZALIA_RESET(0x1),
	/* HDA Codec Subsystem ID Verb-table
	   HDA Codec Subsystem ID  : 0x10280A20 */
	0x00172020,
	0x0017210A,
	0x00172228,
	0x00172310,
	/* Pin Widget Verb-table */
	/* Widget node   0x01 : Widget Reset */
	0x0017FF00,
	0x0017FF00,
	0x0017FF00,
	0x0017FF00,
	/* Pin widget 0x12 - DMIC1-2 */
	0x01271C40,
	0x01271D01,
	0x01271EA6,
	0x01271F90,
	/* Pin widget 0x13 - DMIC3-4 */
	0x01371C00,
	0x01371D00,
	0x01371E00,
	0x01371F40,
	/* Pin widget 0x14 - FRONT (Port-D) */
	0x01471C10,
	0x01471D01,
	0x01471E17,
	0x01471F90,
	/* Pin widget 0x18 - MIC1 */
	0x01871CF0,
	0x01871D11,
	0x01871E11,
	0x01871F41,
	/* Pin widget 0x19 - MIC2 (Port-F) */
	0x01971CF0,
	0x01971D11,
	0x01971E11,
	0x01971F41,
	/* Pin widget 0x1A - LINE1 (Port-C) */
	0x01A71CF0,
	0x01A71D11,
	0x01A71E11,
	0x01A71F41,
	/* Pin widget 0x1B - LINE2 (Port-E) */
	0x01B71CF0,
	0x01B71D11,
	0x01B71E11,
	0x01B71F41,
	/* Pin widget 0x1D - BEEP-IN */
	0x01D71C01,
	0x01D71D00,
	0x01D71E70,
	0x01D71F40,
	/* Pin widget 0x1E - S/PDIF-OUT1 (Define special SKU for driver) */
	0x01E71CF2,
	0x01E71D12,
	0x01E71E12,
	0x01E71F42,
	/* Pin widget 0x21 - HP-OUT (Port-I) */
	0x02171C20,
	0x02171D10,
	0x02171E21,
	0x02171F02,

	/* RESET to D0 */
	0x00170500,
	0x00170500,
	0x00170500,
	0x00170500,
	/* RESET Register */
	0x0205001A,
	0x02048003,
	0x0205001A,
	0x0204C003,
	/* ALC3204 default-1(Class D RESET) */
	0x0205003C,
	0x02040354,
	0x0205003C,
	0x02040314,
	/* ALC3204 default-2 */
	0x02050040,
	0x02049800,
	0x02050034,
	0x0204023C,
	/* ALC3204 Speaker output power - 4 ohm 2W (+12dB gain)
	   + Combo Jack TRS setting */
	0x02050038,
	0x02043901,
	0x02050045,
	0x0204C489,
	/* H/W AGC setting-1 */
	0x02050016,
	0x02040C50,
	0x02050012,
	0x0204EBC2,
	/* H/W AGC setting-2 */
	0x02050013,
	0x0204401D,
	0x02050016,
	0x02044E50,
	/* Zero data  + EAPD to verb-control */
	0x02050037,
	0x0204FE15,
	0x02050010,
	0x02040020,
	/* Zero data */
	0x02050030,
	0x02048000,
	0x02050030,
	0x02048000,
	/* ALC3204 default-3 */
	0x05750003,
	0x05740DA3,
	0x02050046,
	0x02040004,
	/* ALC3204 default-4 */
	0x0205001B,
	0x02040A4B,
	0x02050008,
	0x02046A6C,
	/* JD1 */
	0x02050009,
	0x0204E003,
	0x0205000A,
	0x02047770,
	/* Microphone + Array MIC  security Disable  +ADC clock Enable */
	0x0205000D,
	0x0204A020,
	0x02050005,
	0x02040700,
	/* Speaker Enable */
	0x0205000C,
	0x020401EF,
	0x0205000C,
	0x020401EF,
	/* EQ Bypass + EQ HPF cutoff 250Hz */
	0x05350000,
	0x0534201A,
	0x0535001d,
	0x05340800,
	/* EQ-2 */
	0x0535001e,
	0x05340800,
	0x05350003,
	0x05341EF8,
	/* EQ-3 */
	0x05350004,
	0x05340000,
	0x05450000,
	0x05442000,
	/* EQ-4 */
	0x0545001d,
	0x05440800,
	0x0545001e,
	0x05440800,
	/* EQ-5 */
	0x05450003,
	0x05441EF8,
	0x05450004,
	0x05440000,
	/* EQ Update */
	0x05350000,
	0x0534E01A,
	0x05350000,
	0x0534E01A,
};

const u32 pc_beep_verbs[] = {
	/* PCBeep pass through to NID14 for ePSA test-1 */
	0x02050036,
	0x02047717,
	0x02050036,
	0x02047717,
	/* PCBeep pass through to NID14 for ePSA test-2 */
	0x01470740,
	0x0143B000,
	0x01470C02,
	0x01470C02,
};

AZALIA_ARRAY_SIZES;

#endif
