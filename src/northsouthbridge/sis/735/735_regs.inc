/*
 * 735_regs.inc:	Register Recommended Setting for SiS 735
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
 *	1. SiS 735 Programming Guide, Rev. 1.0, Apr. 16, 2001
 *
 * $Id$
 */

northbridge_init_table:
#	High Byte -> Register	Low Byte -> Value
	/* BIU Control */
	.word	0x5007
	.word	0x51a4
	.word	0x5277
	.word	0x53f7
	.word	0x54aa
	.word	0x55d4
	.word	0x56e8
	.word	0x57f0

	/* DRAM Control */
	.word	0x5a38
	.word	0x5b03
	.word	0x5e03

	/* ACPI Control */
	.word	0x6a18
	.word	0x6b03

	/* PCI 33 Control */
	.word	0x8002
	.word	0x8140
	.word	0x8210
	.word	0x8300
	.word	0x8415
	.word	0x8500
	.word	0x8680
	.word	0x8701

	/* EDB Control */
	.word	0x8820
	.word	0x890c
	.word	0x8b04

	/* IO Buffer */
	.word	0x9b40
	.word	0x9f10
	.word	0xa041
	.word	0xa111
	.word	0xa221
	.word	0xa301
	.word	0xa4aa
	.word	0xa5aa

	/* S2K Interface */
	.word	0xb836
	.word	0xb99a
	.word	0xba48
	.word	0xbbfa
	.word	0xbc16

	/* PCI 66 Control */
	.word	0xd002
	.word	0xd231
	.word	0xd409
	.word	0xd5ff
	.word	0xd6ff
	.word	0xdd88	
northbridge_init_table_end:
