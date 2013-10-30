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

