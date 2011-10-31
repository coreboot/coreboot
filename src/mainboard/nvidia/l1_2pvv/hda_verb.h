/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Copyright (C) 2006-2007 AMD
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

static u32 mainboard_cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0880,	// Codec Vendor / Device ID: Realtek ALC880
	0x00000000,	// Subsystem ID
	0x0000000d,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x0000e601 */
	0x00172001,
	0x001721e6,
	0x00172200,
	0x00172300,

	/* NID 0x14, FRONT-OUT-L/R */
	0x01471c10,
	0x01471d44,
	0x01471e01,
	0x01471f01,

	/* NID 0x15, SURR-OUT-L/R */
	0x01571c12,
	0x01571d14,
	0x01571e01,
	0x01571f01,

	/* NID 0x16, CEN/LFE-OUT */
	0x01671c11,
	0x01671d60,
	0x01671e01,
	0x01671f01,

	/* NID 0x17, SIDE-SURR-L/R */
	0x01771c14,
	0x01771d20,
	0x01771e01,
	0x01771f01,

	/* NID 0x18, MIC1-L/R, VREFO */
	0x01871c30,
	0x01871d9c,
	0x01871ea1,
	0x01871f01,

	/* NID 0x19, MIC2-L/R, VREFO */
	0x01971c40,
	0x01971d9c,
	0x01971ea1,
	0x01971f02,

	/* NID 0x1a, LINE1-L/R, VREFO */
	0x01a71c31,
	0x01a71d34,
	0x01a71e81,
	0x01a71f01,

	/* NID 0x1b, LINE2-L/R, VREFO */
	0x01b71c1f,
	0x01b71d44,
	0x01b71e21,
	0x01b71f02,

	/* NID 0x1c, CD-L/R / GND */
	0x01c71cf0,
	0x01c71d11,
	0x01c71e11,
	0x01c71f41,

	/* NID 0x1d, PCBEEP */
	0x01d71c3e,
	0x01d71d01,
	0x01d71e83,
	0x01d71f99,

	/* NID 0x1e, S/PDIF-OUT */
	0x01e71c20,
	0x01e71d41,
	0x01e71e45,
	0x01e71f01,

	/* NID 0x1f, S/PDIF-IN */
	0x01f71c50,
	0x01f71d91,
	0x01f71ec5,
	0x01f71f01,
};

extern u32 *cim_verb_data;
extern u32 cim_verb_data_size;

