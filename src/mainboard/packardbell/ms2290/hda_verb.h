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
	0x10ec0272,	/* Codec Vendor / Device ID: Realtek ALC272X */
	0x10250379,	/* Subsystem ID  */
	0x00000006,	/* Number of 4 dword sets */

	/* NID 0x01: Subsystem ID.  */
	0x00172079,
	0x00172103,
	0x00172225,
	0x00172310,

	/* NID 0x14.  */
	0x01471C10,
	0x01471D01,
	0x01471E13,
	0x01471F99,

	/* NID 0x18.  */
	0x01871C30,
	0x01871D18,
	0x01871EA1,
	0x01871F03,

	/* NID 0x19.  */
	0x01971C20,
	0x01971D09,
	0x01971EA3,
	0x01971F99,

	/* NID 0x1D.  */
	0x01D71C2D,
	0x01D71D99,
	0x01D71E17,
	0x01D71F40,

	/* NID 0x21.  */
	0x02171C1F,
	0x02171D10,
	0x02171E21,
	0x02171F03,

	0x80862804,	/* Codec Vendor / Device ID: Intel Ibexpeak HDMI.  */
	0x80860101,	/* Subsystem ID  */
	0x00000004,	/* Number of 4 dword sets */

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x17aa21b5 */
	0x30172001,
	0x30172101,
	0x30172286,
	0x30172380,

	/* NID 0x04.  */
	0x30471C10,
	0x30471D00,
	0x30471E56,
	0x30471F18,

	/* NID 0x05.  */
	0x30571C20,
	0x30571D00,
	0x30571E56,
	0x30571F58,

	/* NID 0x06.  */
	0x30671C30,
	0x30671D00,
	0x30671E56,
	0x30671F58,
};
