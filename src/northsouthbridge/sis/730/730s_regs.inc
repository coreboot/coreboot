/*
 * 730_regs.inc:	Register Recommended Setting for SiS 730S
 *
 *
 * Copyright 2000 Silicon Integrated Systems Corporation
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference:
 *	1. SiS 730S Specification
 *	2. SiS 730S Register Recommended Setting
 *	   Rev 1.0, Oct. 4, 2000
 *
 * $Id$
 */

northbridge_init_table:
#		Reg.	Value
	/* generic PCI configuration space */
	.byte	0x0D,	0x20	# Master Latency Timer = 32 PCI CLCK

	/* SiS 730 specific registers. See SiS 730 Registers Recommended Setting */
	
	/* Host Control Interface */
	.byte	0x50,	0xC5	#
	.byte	0x51,	0x10	#

	/* DRAM Control */
	.byte	0x52,	0x80	#	
	.byte	0x53,	0x00	#
	.byte	0x54,	0x00	# 
	.byte	0x55,	0x01	# 

	.byte	0x57,	0x00	# 

	/* Pre-driver Slew Rate/Current Driving Control */
	.byte	0x58,	0x00	#
	.byte	0x59,	0x05	#
	.byte	0x5A,	0x51	#
	.byte	0x5B,	0x00	#

	.byte	0x65,	0x00	# Use DIMM 0 for SMA

	/* MISC Control */
	.byte	0x6A,	0x66	#
	.byte	0x6B,	0x1E	# 0x00 -> 66/133 MHZ, 0x01 -> 100 MHZ
	.byte	0x6C,	0x03	# 0x2E -> 66 MHZ, 0x20 -> 100 MHZ, 0x2C -> 133 MHZ
 
	/* PCI Interface */
	.byte	0x80,	0x72	#
	.byte	0x81,	0x07	#
	.byte	0x82,	0xFF	#
	.byte	0x83,	0xFF	#

	.byte	0x84,	0x83	#
	.byte	0x85,	0x24	#
	.byte	0x86,	0x00	#
	.byte	0x87,	0x00	#

	.byte	0x88,	0xF0	#
	.byte	0x89,	0xF0	#
	.byte	0x8A,	0xF0	#
	.byte	0x8B,	0xF0	#

	/* AGP GART Base Address */
	.byte	0x90,	0x00
	.byte	0x91,	0x00
	.byte	0x92,	0x00
	.byte	0x93,	0x00

	/* Graphic Window Size */
	.byte	0x94,	0x40	# Graphic Window Size == 64MB
	.byte	0x95,	0x00	# 
	.byte	0x96,	0x00	# 
	.byte	0x97,	0x01	# 

	.byte	0x9C,	0x02	# Integrated VGA Enable

	/* DRAM Priority Timer Control REgister */
	.byte	0xA0,	0x00
	.byte	0xA1,	0x00
	.byte	0xA2,	0x23
	.byte	0xA3,	0x43

	.byte	0xD0,	0x0A
	.byte	0xD1,	0x60
	.byte	0xD2,	0x60
	.byte	0xD3,	0x88

	.byte	0xD4,	0xFF
	.byte	0xD5,	0xFF
	.byte	0xD6,	0x88
	.byte	0xD3,	0x11

northbridge_init_table_end: