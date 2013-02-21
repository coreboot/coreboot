/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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
	0x11020011,	// Codec Vendor / Device ID: Creative CA0132
	0x144dc0c2,	// Subsystem ID
	0x00000014,	// Number of jacks + Number of Malcolm setup blocks.

	/* Malcolm Setup */

	0x01570d09,
	0x01570c23,
	0x01570a01,
	0x01570df0,

	0x01570efe,
	0x01570775,
	0x015707d3,
	0x01570709,

	0x01570753,
	0x015707d4,
	0x015707ef,
	0x01570775,

	0x015707d3,
	0x01570709,
	0x01570702,
	0x01570737,

	0x01570778,
	0x01553cce,
	0x015575c9,
	0x01553dce,

	0x0155b7c9,
	0x01570de8,
	0x01570efe,
	0x01570702,

	0x01570768,
	0x01570762,
	0x01553ace,
	0x015546c9,

	0x01553bce,
	0x0155e8c9,
	0x01570d49,
	0x01570c88,

	0x01570d20,
	0x01570e19,
	0x01570700,
	0x01571a05,

	0x01571b29,
	0x01571a04,
	0x01571b29,
	0x01570a01,

	/* Pin Widget Verb Table */

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x144DC0C2 */
	0x001720c2,
	0x001721c0,
	0x0017224d,
	0x00172314,

	/* Pin Complex (NID 0x0B)  Port-G Analog Unknown  Speaker at Int N/A */
	0x00b71cf0,
	0x00b71d00,
	0x00b71e17,
	0x00b71f90,

	/* Pin Complex (NID 0x0C)  N/C */
	0x00c71cf0,
	0x00c71d00,
	0x00c71ef0,
	0x00c71f70,

	/* Pin Complex (NID 0x0D)  N/C */
	0x00d71cf0,
	0x00d71d00,
	0x00d71ef0,
	0x00d71f70,

	/* Pin Complex (NID 0x0E)  N/C */
	0x00e71cf0,
	0x00e71d00,
	0x00e71ef0,
	0x00e71f70,

	/* Pin Complex (NID 0x0F)  N/C */
	0x00f71cf0,
	0x00f71d00,
	0x00f71ef0,
	0x00f71f70,

	/* Pin Complex (NID 0x10)  Port-D 1/8 Black HP Out at Ext Left */
	0x01071cf0,
	0x01071d10,
	0x01071e21,
	0x01071f03,

	/* Pin Complex (NID 0x11) Port-B Click Mic */
	0x01171cf0,
	0x01171d00,
	0x01171ea7,
	0x01171f90,

	/* Pin Complex (NID 0x12) Port-C Combo Jack Mic or D-Mic */
	0x01271cf0,
	0x01271d10,
	0x01271ea1,
	0x01271f03,

	/* Pin Complex (NID 0x13) What you hear */
	0x01371cf0,
	0x01371d00,
	0x01371ed6,
	0x01371f90,

	/* coreboot specific header */
	0x80862806,	// Codec Vendor / Device ID: Intel CougarPoint HDMI
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

	/* Pin Complex (NID 0x06) Digital Out at Int HDMI */
	0x30671c20,
	0x30671d00,
	0x30671e56,
	0x30671f18,

	/* Pin Complex (NID 0x07) Digital Out at Int HDMI */
	0x30771c30,
	0x30771d00,
	0x30771e56,
	0x30771f18
};


static const u32 mainboard_pc_beep_verbs[] = {
	0x00170500,			/* power up codec */
	0x00270500,			/* power up DAC */
	0x00b70500,			/* power up speaker */
	0x00b70740,			/* enable speaker out */
	0x00b78d00,			/* enable EAPD pin */
	0x00b70c02,			/* set EAPD pin */
	0x0143b013,			/* beep volume */
};
static const u32 mainboard_pc_beep_verbs_size =
	sizeof(mainboard_pc_beep_verbs) / sizeof(mainboard_pc_beep_verbs[0]);
