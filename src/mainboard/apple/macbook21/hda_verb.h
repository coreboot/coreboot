/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Vladimir Serbinenko.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License,
 * or (at your option) any later version.
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
	0x83847680,	/* Codec Vendor / Device ID: SigmaTel STAC9221 A1 */
	0x106b2200,	/* Subsystem ID  */
	0x0000000B,	/* Number of 4 dword sets */

	/* NID 0x01: Subsystem ID.  */
	0x00172000,
	0x00172122,
	0x0017226B,
	0x00172310,

	/* NID 0x0A.  */
	0x00A71C1F,
	0x00A71DE2,
	0x00A71E21,
	0x00A71F03,

	/* NID 0x0B.  */
	0x00B71C2E,
	0x00B71DE0,
	0x00B71EA1,
	0x00B71F03,

	/* NID 0x0C.  */
	0x00C71C10,
	0x00C71DE1,
	0x00C71E17,
	0x00C71F90,

	/* NID 0x0D.  */
	0x00D71C1F,
	0x00D71DE1,
	0x00D71E17,
	0x00D71F90,

	/* NID 0x0E.  */
	0x00E71CFE,
	0x00E71D00,
	0x00E71E00,
	0x00E71F40,

	/* NID 0x0F  */
	0x00F71C20,
	0x00F71DE0,
	0x00F71E81,
	0x00F71F03,

	/* NID 0x10  */
	0x01071C30,
	0x01071DE2,
	0x01071E45,
	0x01071F13,

	/* NID 0x11  */
	0x01171C40,
	0x01171DE2,
	0x01171EC5,
	0x01171F13,

	/* NID 0x15  */
	0x01571CFC,
	0x01571D00,
	0x01571E00,
	0x01571F40,

	/* NID 0x1B.  */
	0x01B71CFB,
	0x01B71D00,
	0x01B71E00,
	0x01B71F40,

};
