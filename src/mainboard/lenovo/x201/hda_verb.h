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
	0x14F15069,	/* Codec Vendor / Device ID: Conexant CX20585 */
	0x17AA2155,	/* Subsystem ID  */
	0x0000000B,	/* Number of 4 dword sets */

	/* NID 0x01: Subsystem ID.  */
	0x00172055,
	0x00172121,
	0x001722AA,
	0x00172317,

	/* NID 0x19: Headphone jack.  */
	0x01971CF0,
	0x01971D40,
	0x01971E21,
	0x01971F04,

	/* NID 0x1A: Dock mic jack.  */
	0x01A71CF0,
	0x01A71D90,
	0x01A71EA1,
	0x01A71F61,

	/* NID 0x1B: Mic jack.  */
	0x01B71CF0,
	0x01B71D90,
	0x01B71EA1,
	0x01B71F04,

	/* NID 0x1C: Dock headphone jack.  */
	0x01C71CF0,
	0x01C71D40,
	0x01C71E21,
	0x01C71F61,

	/* NID 0x1D: EAPD detect.  */
	0x01D71CF0,
	0x01D71D00,
	0x01D71E17,
	0x01D71F60,

	/* NID 0x1E  */
	0x01E71CF0,
	0x01E71D01,
	0x01E71EF0,
	0x01E71F40,

	/* NID 0x1F  */
	0x01F71CF0,
	0x01F71D01,
	0x01F71E17,
	0x01F71F90,

	/* NID 0x20  */
	0x02071CF0,
	0x02071D01,
	0x02071EF0,
	0x02071F40,

	/* NID 0x22  */
	0x02271CF0,
	0x02271D01,
	0x02271EF0,
	0x02271F40,

	/* NID 0x23: Internal mic boost volume.  */
	0x02371CF0,
	0x02371D01,
	0x02371EA6,
	0x02371F90,

	0x80862804,	/* Codec Vendor / Device ID: Intel Ibexpeak HDMI.  */
	0x17aa21b5,	/* Subsystem ID  */
	0x00000004,	/* Number of 4 dword sets */

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x17aa21b5 */
	0x301720B5,
	0x30172121,
	0x301722AA,
	0x30172317,

	/* NID 0x04.  */
	0x30471C10,
	0x30471D00,
	0x30471E56,
	0x30471F58,

	/* NID 0x05.  */
	0x30571C20,
	0x30571D00,
	0x30571E56,
	0x30571F18,

	/* NID 0x06.  */
	0x30671C30,
	0x30671D00,
	0x30671E56,
	0x30671F58,
};
