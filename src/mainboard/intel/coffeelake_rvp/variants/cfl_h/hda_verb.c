/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/*
	 * VerbTable: CFL Display Audio Codec
	 * Revision ID = 0xFF
	 * Codec Vendor: 0x8086280B
	 */

	0x8086280B,
	0xFFFFFFFF,
	0x00000005,

	/*
	 * Display Audio Verb Table
	 * For GEN9, the Vendor Node ID is 08h
	 * Port to be exposed to the inbox driver in the vanilla mode
	 * PORT C - BIT[7:6] = 01b
	 */
	0x00878140,
	0x00878140,
	0x00878140,
	0x00878140,
	/* Pin Widget 5 - PORT B - Configuration Default: 0x18560010 */
	0x00571C10,
	0x00571D00,
	0x00571E56,
	0x00571F18,
	/* Pin Widget 6 - PORT C - Configuration Default: 0x18560020 */
	0x00671C20,
	0x00671D00,
	0x00671E56,
	0x00671F18,
	/* Pin Widget 7 - PORT D - Configuration Default: 0x18560030 */
	0x00771C30,
	0x00771D00,
	0x00771E56,
	0x00771F18,
	/* Disable the third converter and third Pin (NID 08h) */
	0x00878140,
	0x00878140,
	0x00878140,
	0x00878140,

	/* ALC700 */
	0x10EC0700,
	0xFFFFFFFF,
	22,

	/*
	 * HDA Codec Subsystem ID Verb-table
	 * HDA Codec Subsystem ID : 0x10EC112C
	 */
	0x0017202C,
	0x00172111,
	0x001722EC,
	0x00172310,

	/*
	 * Pin Widget Verb-table
	 * Widget node 0x01
	 */
	0x0017FF00,
	0x0017FF00,
	0x0017FF00,
	0x0017FF00,
	/* Pin widget 0x12 - DMIC */
	0x01271C00,
	0x01271D00,
	0x01271E00,
	0x01271F40,
	/* Pin widget 0x13 - DMIC */
	0x01371CF0,
	0x01371D11,
	0x01371E11,
	0x01371F41,
	/* Pin widget 0x14 - FRONT (Port-D) */
	0x01471CF0,
	0x01471D11,
	0x01471E11,
	0x01471F41,
	/* Pin widget 0x15 - I2S-OUT */
	0x01571C10,
	0x01571D01,
	0x01571E17,
	0x01571F90,
	/* Pin widget 0x16 - LINE3 (Port-B) */
	0x01671C20,
	0x01671D10,
	0x01671E01,
	0x01671F01,
	/* Pin widget 0x17 - I2S-OUT */
	0x01771CF0,
	0x01771D11,
	0x01771E11,
	0x01771F41,
	/* Pin widget 0x18 - I2S-IN */
	0x01871CF0,
	0x01871D11,
	0x01871E11,
	0x01871F41,
	/* Pin widget 0x19 - MIC2 (Port-F) */
	0x01971C30,
	0x01971D90,
	0x01971EA1,
	0x01971F02,
	/* Pin widget 0x1A - LINE1 (Port-C) */
	0x01A71CF0,
	0x01A71D11,
	0x01A71E11,
	0x01A71F41,
	/* Pin widget 0x1B - LINE2 (Port-E) */
	0x01B71C40,
	0x01B71D90,
	0x01B71EA1,
	0x01B71F01,
	/* Pin widget 0x1D - PC-BEEP */
	0x01D71C69,
	0x01D71D84,
	0x01D71E45,
	0x01D71F40,
	/* Pin widget 0x1E - S/PDIF-OUT */
	0x01E71CF0,
	0x01E71D11,
	0x01E71E11,
	0x01E71F41,
	/* Pin widget 0x1F - S/PDIF-IN */
	0x01F71CF0,
	0x01F71D11,
	0x01F71E11,
	0x01F71F41,
	/* Pin widget 0x21 - P-OUT (Port-I) */
	0x02171C2F,
	0x02171D10,
	0x02171E21,
	0x02171F02,
	/* Pin widget 0x29 - I2S-IN */
	0x02971CF0,
	0x02971D11,
	0x02971E11,
	0x02971F41,
	/*
	 * Widget node 0x20 :  MIC2-Vrefo-R and MIC2-vrefo-L
	 * to independent control
	 */
	0x02050045,
	0x02045089,
	0x0205004A,
	0x0204201B,
	/* Widget node 0x20 - 1 */
	0x05850000,
	0x05843888,
	0x0205006F,
	0x02042C0B,
	/*
	 * Widget node 0x20 - 2 : Line2-JD gating MIC2-Vrefo-R,
	 * P-JD gating MIC2-vrefo-L
	 */
	0x0205006B,
	0x02044260,
	0x05B50010,
	0x05B45C1D,

	/* Widget node 0X20 for ALC1305 */
	0x02050024,
	0x02040010,
	0x02050026,
	0x02040000,
	0x02050028,
	0x02040000,
	0x02050029,
	0x0204B024,

};

const u32 pc_beep_verbs[] = {
};

AZALIA_ARRAY_SIZES;
