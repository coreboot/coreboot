/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

	.section ".romstrap", "a", @progbits


	.globl __romstrap_start
__romstrap_start:
rstables:
	.long 0x2b16d065
	.long 0x0
	.long 0x0
	.long linkedlist

linkedlist:
	.long 0x0003001C			// 10h
	.long 0x08000000			// 14h
	.long 0x00000000			// 18h
	.long 0xFFFFFFFF			// 1Ch

	.long 0xFFFFFFFF			// 20h
	.long 0xFFFFFFFF			// 24h
	.long 0xFFFFFFFF			// 28h
	.long 0xFFFFFFFF			// 2Ch

	.long 0x56341200			// 30h, MAC address low 4 byte ---> keep it in 0xffffffc0
	.long 0x00009078			// 34h, MAC address high 4 byte

	.long 0x002309CE			// 38h, UUID low 4 byte
	.long 0x00E08100			// 3Ch, UUID high 4 byte

	.long 0x00402000			//Firmware trap for SiS761+966
	.long 0xE043A800
	.long 0x00180000
	.long 0x1421C402

rspointers:
	.long rstables				// It will be 0xffffffe0
	.long rstables
	.long rstables
	.long rstables

	.globl __romstrap_end

__romstrap_end:
.previous
