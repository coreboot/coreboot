/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0269,	// Codec Vendor / Device ID: Realtek
	0x17aa21fe,	// Subsystem ID
	0x0000000c,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x17aa21fe */
	AZALIA_SUBVENDOR(0x0, 0x17aa21fe),

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x12): 0x90a60930 DMIC */
	AZALIA_PIN_CFG(0x0, 0x12, 0x90a60930),

	/* Pin Complex (NID 0x14): 0x90170110 SPEAKER-OUT (Port-D) */
	AZALIA_PIN_CFG(0x0, 0x14, 0x90170110),

	/* Pin Complex (NID 0x15): 0x0321101f HP-OUT (Port-A) */
	AZALIA_PIN_CFG(0x0, 0x15, 0x0321101f),

	/* Pin Complex (NID 0x17): 0x411111f0 MONO-OUT (Port-H) */
	AZALIA_PIN_CFG(0x0, 0x17, 0x411111f0),

	/* Pin Complex (NID 0x18): 0x03a11820 MIC1 (Port-B) */
	AZALIA_PIN_CFG(0x0, 0x18, 0x03a11820),

	/* Pin Complex (NID 0x19): 0x411111f0 MIC2 (Port-F) */
	AZALIA_PIN_CFG(0x0, 0x19, 0x411111f0),

	/* Pin Complex (NID 0x1a): 0x411111f0 LINE1 (Port-C) */
	AZALIA_PIN_CFG(0x0, 0x1a, 0x411111f0),

	/* Pin Complex (NID 0x1b): 0x411111f0 LINE2 (Port-E) */
	AZALIA_PIN_CFG(0x0, 0x1b, 0x411111f0),

	/* Pin Complex (NID 0x1d): 0x4016862d PC-BEEP */
	AZALIA_PIN_CFG(0x0, 0x1d, 0x4016862d),

	/* Pin Complex (NID 0x1e): 0x411111f0 S/PDIF-OUT */
	AZALIA_PIN_CFG(0x0, 0x1e, 0x411111f0),

	/* Stout MIC detect setup */
	0x02050018,
	0x02045184,
	0x02050008,
	0x02040300,

	/* coreboot specific header */
	0x80862806,	// Codec Vendor / Device ID: Intel PantherPoint HDMI
	0x80860101,	// Subsystem ID
	0x00000004,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x80860101 */
	AZALIA_SUBVENDOR(0x3, 0x80860101),

	/* Pin Complex (NID 0x05) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(0x3, 0x05, 0x18560010),

	/* Pin Complex (NID 0x06) Not Connected */
	AZALIA_PIN_CFG(0x3, 0x06, 0x58560020),

	/* Pin Complex (NID 0x07) Not Connected */
	AZALIA_PIN_CFG(0x3, 0x07, 0x58560030)
};

const u32 pc_beep_verbs[] = {
	0x00170500,	/* power up everything (codec, dac, adc, mixers)  */
	0x01470740,	/* enable speaker out */
	0x01470c02,	/* set speaker EAPD pin */
	0x0143b01f,	/* unmute speaker */
	0x00c37100,	/* unmute mixer nid 0xc input 1 */
	0x00b37410,	/* unmute mixer nid 0xb beep input and set volume */
};
AZALIA_ARRAY_SIZES;
