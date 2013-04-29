/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012-2013 secunet Security Networks AG
 * Copyright (C) 2013 Nico Huber <nico.h@gmx.de>
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

/*
 * Pin widget configuration:
 *
 * Port-Con.  Location  Def.-Dev.  Con.-Type   Color    Misc  Def.-Aso.     Seq
 *    31..30    29..24     23..20     19..16  15..12  11..00     07..04  03..00
 */
#define PIN_CFG(pin, val) \
	(pin << 20) | ( 0x71c << 8) | (val & 0xff), \
	(pin << 20) | ( 0x71d << 8) | ((val >> 8) & 0xff), \
	(pin << 20) | ( 0x71e << 8) | ((val >> 16) & 0xff), \
	(pin << 20) | ( 0x71f << 8) | ((val >> 24) & 0xff), \

static const u32 mainboard_cim_verb_data[] = {
	/* coreboot specific header */
	0x11060397,	// Codec Vendor / Device ID: Via VT1708S
	0x11060000,	// Subsystem ID
	0x0000000c,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x11060000 */
	0x00172000,
	0x00172100,
	0x00172206,
	0x00172311,

	/* Pin Widget Verb Table */

	/*
	 * NID 0x19 [Port A (SURR)]:
	 * Jack     Internal    Speaker     N/A             Black
	 */
	PIN_CFG(0x19, 0x10101112)

	/*
	 * NID 0x1a [Port B (MIC1/2)]:
	 * Jack     Rear        Mic In      1/8"            Pink
	 */
	PIN_CFG(0x1a, 0x01a19036)

	/*
	 * NID 0x1b [Port C (LINEIN)]:
	 * Jack     Rear        Line In     1/8"            Blue
	 */
	PIN_CFG(0x1b, 0x0181303e)

	/*
	 * NID 0x1c [Port D (Front)]:
	 * Jack     Rear        Line Out    1/8"            Green
	 */
	PIN_CFG(0x1c, 0x01014010)

	/*
	 * NID 0x1d [Port E (Front HP/MIC)]:
	 * Jack     Front       HP Out      1/8"            Green
	 */
	PIN_CFG(0x1d, 0x022141f0)

	/*
	 * NID 0x1e [Port F (Front HP/MIC)]:
	 * Jack     Front       Mic In      1/8"            Pink
	 */
	PIN_CFG(0x1e, 0x02a19138)

	/*
	 * NID 0x1f [CD]:
	 * Jack     Int.(ATAPI) CD          ATAPI internal  Black
	 */
	PIN_CFG(0x1f, 0x19331137)

	/*
	 * NID 0x20 [N/A]:
	 * Jack     Rear        S/PDIF Out  RCA             Unknown
	 */
	PIN_CFG(0x20, 0x014401f0)

	/*
	 * NID 0x21 [N/A]:
	 * None     Internal    S/PDIF Out  Other Digital   Unknown
	 */
	PIN_CFG(0x21, 0x504600f0)

	/*
	 * NID 0x22 [Port G (C/LFE)]:
	 * Jack     Internal    Speaker     Unknown         Orange
	 */
	PIN_CFG(0x22, 0x10106111)

	/*
	 * NID 0x23 [Port H (SSL/SSR)]:
	 * Jack     Internal    Speaker     Unknown         Grey
	 */
	PIN_CFG(0x23, 0x10102114)


	/* coreboot specific header */
	0x80862806,	// Codec Vendor / Device ID: Intel PantherPoint HDMI
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
	0x30571f58,

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
