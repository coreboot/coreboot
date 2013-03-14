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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

static const u32 mainboard_cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0269,	// Codec Vendor / Device ID: Realtek
	0x17aa21fe,	// Subsystem ID
	0x0000000c,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x17aa21fe */
	0x001720fe,
	0x00172121,
	0x001722aa,
	0x00172317,

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x12): 0x90a60930 DMIC */
	0x01271c30,
	0x01271d09,
	0x01271ea6,
	0x01271f90,

	/* Pin Complex (NID 0x14): 0x90170110 SPEAKER-OUT (Port-D) */
	0x01471c10,
	0x01471d01,
	0x01471e17,
	0x01471f90,

	/* Pin Complex (NID 0x15): 0x0321101f HP-OUT (Port-A) */
	0x01571c1f,
	0x01571d10,
	0x01571e21,
	0x01571f03,

	/* Pin Complex (NID 0x17): 0x411111f0 MONO-OUT (Port-H) */
	0x01771cf0,
	0x01771d11,
	0x01771e11,
	0x01771f41,

	/* Pin Complex (NID 0x18): 0x03a11820 MIC1 (Port-B) */
	0x01871c20,
	0x01871d18,
	0x01871ea1,
	0x01871f03,

	/* Pin Complex (NID 0x19): 0x411111f0 MIC2 (Port-F) */
	0x01971cf0,
	0x01971d11,
	0x01971e11,
	0x01971f41,

	/* Pin Complex (NID 0x1a): 0x411111f0 LINE1 (Port-C) */
	0x01a71cf0,
	0x01a71d11,
	0x01a71e11,
	0x01a71f41,

	/* Pin Complex (NID 0x1b): 0x411111f0 LINE2 (Port-E) */
	0x01b71cf0,
	0x01b71d11,
	0x01b71e11,
	0x01b71f41,

	/* Pin Complex (NID 0x1d): 0x4016862d PC-BEEP */
	0x01d71c2d,
	0x01d71d86,
	0x01d71e16,
	0x01d71f40,

	/* Pin Complex (NID 0x1e): 0x411111f0 S/PDIF-OUT */
	0x01e71cf0,
	0x01e71d11,
	0x01e71e11,
	0x01e71f41,

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
	0x30172001,
	0x30172101,
	0x30172286,
	0x30172380,

	/* Pin Complex (NID 0x05) Digital Out at Int HDMI */
	0x30571c10,
	0x30571d00,
	0x30571e56,
	0x30571f18,

	/* Pin Complex (NID 0x06) Not Connected */
	0x30671c20,
	0x30671d00,
	0x30671e56,
	0x30671f58,

	/* Pin Complex (NID 0x07) Not Connected */
	0x30771c30,
	0x30771d00,
	0x30771e56,
	0x30771f58
};

static const u32 mainboard_pc_beep_verbs[] = {
	0x00170500,	/* power up everything (codec, dac, adc, mixers)  */
	0x01470740,	/* enable speaker out */
	0x01470c02,	/* set speaker EAPD pin */
	0x0143b01f,	/* unmute speaker */
	0x00c37100,	/* unmute mixer nid 0xc input 1 */
	0x00b37410,	/* unmute mixer nid 0xb beep input and set volume */
};
static const u32 mainboard_pc_beep_verbs_size =
	sizeof(mainboard_pc_beep_verbs) / sizeof(mainboard_pc_beep_verbs[0]);
