/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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
	0x10ec0262,     // Codec Vendor / Device ID: Realtek ALC262
	0x10714700,     // Subsystem ID
	0x0000000f,     // Number of jacks

	/* ===== HDA Codec Subsystem ID Verb-table ===== */
	/* HDA Codec Subsystem ID  : 0x10EC0000 */
	0x00172000,
	0x00172100,
	0x001722ec,
	0x00172310,

	/* ===== Pin Widget Verb-table ===== */
	/* Widget node 0x01 : */
	0x0017ff00,
	0x0017ff00,
	0x0017ff00,
	0x0017ff00,
	/* Pin widget 0x11 - S/PDIF-OUT2 */
	0x01171c00,
	0x01171d00,
	0x01171e00,
	0x01171f40,
	/* Pin widget 0x12 - DMIC */
	0x01271cf0,
	0x01271d11,
	0x01271e11,
	0x01271f41,
	/* Pin widget 0x14 - LINE-OUT (Port-D) */
	0x01471c10,
	0x01471d41,
	0x01471e01,
	0x01471f01,
	/* Pin widget 0x15 - HP-OUT (Port-A) */
	0x01571cf0,
	0x01571d11,
	0x01571e11,
	0x01571f41,
	/* Pin widget 0x16 - MONO-OUT */
	0x01671cf0,
	0x01671d11,
	0x01671e11,
	0x01671f41,
	/* Pin widget 0x18 - MIC1 (Port-B) */
	0x01871cf0,
	0x01871d11,
	0x01871e11,
	0x01871f41,
	/* Pin widget 0x19 - MIC2 (Port-F) */
	0x01971c30,
	0x01971d91,
	0x01971ea1,
	0x01971f02,
	/* Pin widget 0x1A - LINE1 (Port-C) */
	0x01a71c40,
	0x01a71d31,
	0x01a71e81,
	0x01a71f01,
	/* Pin widget 0x1B - LINE2 (Port-E) */
	0x01b71cf0,
	0x01b71d11,
	0x01b71e11,
	0x01b71f41,
	/* Pin widget 0x1C - CD-IN */
	0x01c71cf0,
	0x01c71d11,
	0x01c71e11,
	0x01c71f41,
	/* Pin widget 0x1D - BEEP-IN */
	0x01d71c29,
	0x01d71d46,
	0x01d71e35,
	0x01d71f40,
	/* Pin widget 0x1E - S/PDIF-OUT */
	0x01e71c20,
	0x01e71d11,
	0x01e71e56,
	0x01e71f18,
	/* Pin widget 0x1F - S/PDIF-IN */
	0x01f71cf0,
	0x01f71d11,
	0x01f71e11,
	0x01f71f41,
};

extern const u32 *cim_verb_data;
extern u32 cim_verb_data_size;
