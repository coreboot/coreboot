/*
 * 630s_regs.inc:	Register Recommended Setting for SiS 630S
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
 *	1. SiS 630S Specification
 *	2. SiS 630S Register Recommended Setting
 *	   Rev 0.91, September, 2000
 *
 * $Id$
 */

northbridge_init_table:
#		Reg.	Value
	/* generic PCI configuration space */
	.byte	0x04,	0x07	# Turn on Bus Master,
	.byte	0x05,	0x00	# Memory and I/O Space
	.byte	0x06,	0x00	# clear PCI status
	.byte	0x07,	0x00	# 	
	.byte	0x0D,	0x20	# Master Latency Timer = 32 PCI CLCK

	/* SiS 630 specific registers. See SiS 630 Registers Recommended Setting */
	
	/* Host Control Interface */
	.byte	0x50,	0x7E	#
	.byte	0x51,	0x13	#

	/* DRAM Control */
	.byte	0x52,	0xC5	#	
	.byte	0x53,	0x0b	# FixMe: Confilct with document

	.byte	0x54,	0x00	# 0x00 -> 66/100 MHZ, 0x08 -> 133 MHZ
	.byte	0x55,	0x05	# 0x05 -> 66/100 MHZ, 0x01 -> 133 MHZ
	.byte	0x56,	0x80	# 0x00 -> 66  MHZ,    0x80 -> 100/133 MHZ
	.byte	0x57,	0x00	# 0x00 -> 66  MHZ     0x01 -> 100/133 MHZ

	/* Pre-driver Slew Rate/Current Driving Control */
	.byte	0x58,	0x00	#
	.byte	0x59,	0x05	#
	.byte	0x5A,	0x51	# 0x51 -> 1 or 2 DIMM, 0x55 -> 3 DIMM 
	.byte	0x5B,	0x00	#

	.byte	0x65,	0x00	# Use DIMM 0 for SMA

	/* MISC Control */
	.byte	0x6A,	0xC6	#
	.byte	0x6B,	0x00	#
	.byte	0x6C,	0x2F	# 0x2E -> 66 MHZ, 0x20 -> 100 MHZ, 0x2C -> 133 MHZ
 
	/* PCI Interface */
	.byte	0x80,	0x23	#
	.byte	0x81,	0xFF	#
	.byte	0x82,	0x7F	#
	.byte	0x83,	0x1F	#

	.byte	0x84,	0xF0	#
	.byte	0x85,	0x00	#
	.byte	0x86,	0x01	#
	.byte	0x87,	0x40	#

	.byte	0x88,	0x00	#
	.byte	0x89,	0x08	#

	/* AGP GART Base Address */
	.byte	0x90,	0x00
	.byte	0x91,	0x00
	.byte	0x92,	0x00
	.byte	0x93,	0x00

	/* Graphic Window Size */
	.byte	0x94,	0x40	# Graphic Window Size == 64MB
	.byte	0x97,	0x01	# Page Table Cache Enable
	.byte	0x98,	0x00	#
	.byte	0x9C,	0x80	# Enable Integrated VGA

	/* DRAM Priority Timer Control REgister */
	.byte	0xA0,	0x00
	.byte	0xA1,	0x00
	.byte	0xA2,	0x03
	.byte	0xA3,	0x01

	/* AGP Command Register */
	.byte	0xC8,	0x04	# AGP 4X
	.byte	0xC9,	0x00	# AGP Disabled
	.byte	0xCA,	0x00
	.byte	0xCB,	0x00

	.byte	0xD0,	0x48
	.byte	0xD1,	0x81
	.byte	0xD2,	0x40
	.byte	0xD3,	0x5E

	.byte	0xD4,	0x0D
	.byte	0xD6,	0x00
	.byte	0xD7,	0x80

	.byte	0xD8,	0x60
	.byte	0xD9,	0x60
	.byte	0xDA,	0xAA
	.byte	0xDB,	0x06

	.byte	0xDC,	0x40
	.byte	0xDD,	0x00
	.byte	0xDE,	0x40

	.byte	0xE0,	0x88
	.byte	0xE1,	0x01
	.byte	0xE2,	0x03

northbridge_init_table_end:
