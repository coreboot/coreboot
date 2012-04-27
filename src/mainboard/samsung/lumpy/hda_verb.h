/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
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
	0x10134210,	// Codec Vendor / Device ID: Realtek ALC262
	0x144db082,	// Subsystem ID
	0x00000007,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x144dc0c2 */
	0x00172082,
	0x001721b0,
	0x0017224d,
	0x00172314,

	0x00170500,

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x05) */
	0x00571cf0,
	0x00571d10,
	0x00571e21,
	0x00571f02,

	/* Pin Complex (NID 0x06) */
	0x00671cf0,
	0x00671d00,
	0x00671e17,
	0x00671f90,

	/* Pin Complex (NID 0x07) */
	0x00771cf0,
	0x00771d10,
	0x00771ea1,
	0x00771f02,

	/* Pin Complex (NID 0x08) */
	0x00871c37,
	0x00871d00,
	0x00871ea7,
	0x00871f77,

	/* Pin Complex (NID 0x09) */
	0x00971c3e,
	0x00971d00,
	0x00971ea6,
	0x00971fb7,

	/* Pin Complex (NID 0x0a) */
	0x00a71cf0,
	0x00a71d10,
	0x00a71e45,
	0x00a71f43
};

static const u32 mainboard_pc_beep_verbs[] = {
	0x00170500,			/* power up codec */
	0x00270500,			/* power up DAC */
	0x00670500,			/* power up speaker */
	0x00670740,			/* enable speaker output */
	0x0023B04B,			/* set DAC gain */
};
static const u32 mainboard_pc_beep_verbs_size =
	sizeof(mainboard_pc_beep_verbs) / sizeof(mainboard_pc_beep_verbs[0]);
