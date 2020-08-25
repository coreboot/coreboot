/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <types.h>

const u32 cim_verb_data[] = {
	0x10ec0888,	/* Codec Vendor / Device ID: Realtek ALC888 */
	0x10ec0888,	/* Subsystem ID */
	17,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x10ec0888),

	/* Port configuration control */

	0x018707e1, /* port B VREF 50% of LDO-OUT */
	0x019707e1, /* port F VREF 50% of LDO-OUT */
	0x01b707e1, /* port E VREF 50% of LDO-OUT*/
	0x0205000d, /* pin 37 vrefo hidden register - used as port C vref */

	0x02041000, /* pin 37 vrefo 50% of LDO-OUT */
	0x02041000, /* Dummy entry */
	0x02041000, /* Dummy entry */
	0x02041000, /* Dummy entry */

	/* Pin widgets */
	AZALIA_PIN_CFG(0, 0x11, 0x411111f0), /* SPDIF-OUT2 - disabled */
	AZALIA_PIN_CFG(0, 0x12, 0x411111f0), /* digital MIC - disabled */
	AZALIA_PIN_CFG(0, 0x14, 0x01014430), /* PORT D - rear line out */
	AZALIA_PIN_CFG(0, 0x16, 0x411111f0), /* PORT G - disabled */
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0), /* PORT H - disabled */
	AZALIA_PIN_CFG(0, 0x18, 0x01a19c50), /* PORT B - rear mic in */
	AZALIA_PIN_CFG(0, 0x1c, 0x411111f0), /* CD audio - disabled */
	AZALIA_PIN_CFG(0, 0x1d, 0x4004c601), /* BEEPIN */
	AZALIA_PIN_CFG(0, 0x1e, 0x01452160), /* SPDIF-OUT */
	AZALIA_PIN_CFG(0, 0x1f, 0x01C52170), /* SPDIF-IN */

	/* Config for R02 and older */
	AZALIA_PIN_CFG(0, 0x19, 0x02214c40), /* port F - front hp out */
	AZALIA_PIN_CFG(0, 0x1a, 0x901001f0), /* port C - internal speaker */
	AZALIA_PIN_CFG(0, 0x1b, 0x01813c10), /* port E - rear line in/mic - Blue */
	AZALIA_PIN_CFG(0, 0x15, 0x02a19c20), /* port A - audio hdr input */

	/*
	 * VerbTable: CFL Display Audio Codec
	 * Revision ID = 0xFF
	 * Codec Vendor: 0x8086280B
	 */
	0x8086280B,
	0xFFFFFFFF,
	5,	/* Number of 4 dword sets */

	AZALIA_SUBVENDOR(2, 0x80860101),

	/*
	 * Display Audio Verb Table
	 * For GEN9, the Vendor Node ID is 08h
	 * Port to be exposed to the inbox driver in the vanilla mode
	 * PORT C - BIT[7:6] = 01b
	 */
	0x20878101,

	/* Pin Widget 5 - PORT B - Configuration Default: 0x18560010 */
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	/* Pin Widget 6 - PORT C - Configuration Default: 0x18560020 */
	AZALIA_PIN_CFG(2, 0x06, 0x18560020),
	/* Pin Widget 7 - PORT D - Configuration Default: 0x18560030 */
	AZALIA_PIN_CFG(2, 0x07, 0x18560030),
	/* Disable the third converter and third Pin (NID 08h) */
	0x20878100,

	/* Dummy entries */
	0x20878100,
	0x20878100,
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
