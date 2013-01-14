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
	0x10134210,	// Codec Vendor / Device ID: Cirrus Logic CS4210
	0x10134210,	// Subsystem ID
	0x00000007,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x10134210 */
	0x00172010,
	0x00172142,
	0x00172213,
	0x00172310,

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x05)     1/8   Gray  HP Out at Ext Front */
	0x00571cf0,
	0x00571d20,
	0x00571e21,
	0x00571f02,

	/* Pin Complex (NID 0x06)  Analog Unknown  Speaker at Int N/A */
	0x00671c10,
	0x00671d00,
	0x00671e17,
	0x00671f90,

	/* Pin Complex (NID 0x07)     1/8    Grey  Line In at Ext Front */
	0x00771cf0,
	0x00771d20,
	0x00771ea1,
	0x00771f02,

	/* Pin Complex (NID 0x08)  Analog Unknown  Mic at Oth Mobile-In */
	0x00871c37,
	0x00871d00,
	0x00871ea7,
	0x00871f77,

	/* Pin Complex (NID 0x09) Digital Unknown  Mic at Oth Mobile-In */
	0x00971c3e,
	0x00971d00,
	0x00971ea6,
	0x00971f77,

	/* Pin Complex (NID 0x0a) Optical   Black  SPDIF Out at Ext N/A */
	0x00a71cf0,
	0x00a71d10,
	0x00a71e45,
	0x00a71f43,

	/* coreboot specific header */
	0x80862805,	// Codec Vendor / Device ID: Intel CougarPoint HDMI
	0x80860101,	// Subsystem ID
	0x00000004,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x80860101 */
	0x00172001,
	0x00172101,
	0x00172286,
	0x00172380,

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

