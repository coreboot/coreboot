/*
 * 635_regs.inc:	Register Recommended Setting for SiS 635
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
 *	1. SiS 635 Programming Giude, Rev. 1.0, Mar. 29, 2001
 *
 * $Id$
 */

northbridge_init_table:
#	High Byte -> Register	Low Byte -> Value
	/* Host Control */
	.word	0x50ff
	.word	0x5147
	.word	0x5271
	.word	0x5383
	.word	0x5403
	.word	0x5501
	.word	0x5688
	.word	0x5788

	/* DRAM Control */
	.word	0x5800
	.word	0x5a08
	.word	0x5b03
	.word	0x5e03

	/* ACPI Control */
	.word	0x6a26
	.word	0x6b00

	/* PCI 33 Control */
	.word	0x8022
	.word	0x8142
	.word	0x8210
	.word	0x8303
	.word	0x8415
	.word	0x8500
	.word	0x8680
	.word	0x8701

	/* EDB Control */
	.word	0x8820
	.word	0x8904
	.word	0x8b04

	/* AGP GART */
	.word	0x9440
	.word	0x9501
	.word	0x9701
	.word	0x9800

	/* IO Buffer */
	.word	0x9b42
	.word	0x9f20
	.word	0xa042
	.word	0xa12a
	.word	0xa222
	.word	0xa301

	/* PCI 66 Control */
	.word	0xd022
	.word	0xd142
	.word	0xd213
	.word	0xd405
	.word	0xd5ff
	.word	0xd6ff
	.word	0xd860
	.word	0xd960
	.word	0xdaaa
	.word	0xdd88	
northbridge_init_table_end:
