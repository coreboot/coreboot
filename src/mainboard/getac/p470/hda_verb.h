/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
	0x10ec0262,	// Codec Vendor / Device ID: Realtek ALC262
	0x10714700,	// Subsystem ID
	0x0000000d,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x10ec0000 */
	0x00172000,
	0x00172100,
	0x001722EC,
	0x00172310,

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x12), DMIC */
	0x01271CF0,
	0x01271D11,
	0x01271E11,
	0x01271F41,

	/* Pin Complex (NID 0x14), LINE_OUT (port D) */
	0x01471C10,
	0x01471D01,
	0x01471E13,
	0x01471F99,

	/* Pin Complex (NID 0x15), HP_OUT (port A) */
	0x01571C20,
	0x01571D40,
	0x01571E21,
	0x01571F01,

	/* Pin Complex (NID 0x16), MONO-OUT */
	0x01671CF0,
	0x01671D11,
	0x01671E11,
	0x01671F41,

	/* Pin Complex (NID 0x18), MIC1 (port B) */
	0x01871C30,
	0x01871D98,
	0x01871EA1,
	0x01871F01,

	/* Pin Complex (NID 0x19), MIC2 (port F) */
	0x01971C31,
	0x01971D09,
	0x01971EA3,
	0x01971F99,

	/* Pin Complex (NID 0x1A), LINE1 (port C) */
	0x01A71C3F,
	0x01A71D98,
	0x01A71EA1,
	0x01A71F02,

	/* Pin Complex (NID 0x1B), LINE2 (port E) */
	0x01B71C1F,
	0x01B71D40,
	0x01B71E21,
	0x01B71F02,

	/* Pin Complex (NID 0x1C), CD_IN */
	0x01C71CF0,
	0x01C71D11,
	0x01C71E11,
	0x01C71F41,

	/* Pin Complex (NID 0x1D), PCBEEP */
	0x01D71CF0,
	0x01D71D11,
	0x01D71E11,
	0x01D71F41,

	/* Pin Complex (NID 0x1E), S/PDIF-OUT */
	0x01E71CF0,
	0x01E71D11,
	0x01E71E11,
	0x01E71F41,

	/* Pin Complex (NID 0x1F), S/PDIF-IN */
	0x01F71CF0,
	0x01F71D11,
	0x01F71E11,
	0x01F71F41,
};

extern const u32 *cim_verb_data;
extern u32 cim_verb_data_size;

