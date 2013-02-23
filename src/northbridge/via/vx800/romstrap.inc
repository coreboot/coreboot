/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer)
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

/* This file constructs the ROM strap table for K8T890 and K8M890 */

	.section ".romstrap", "a", @progbits

	.globl __romstrap_start
__romstrap_start:
tblpointer:
	.long 0x55aa66cc
	.long 0x88012554
	.long 0x77107777
	.long 0x00770814


	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000

/*
 * The pointer to above table should be at 0xffffffd0,
 * the table itself MUST be aligned to 128B it seems!
 */
rspointers:
	.long tblpointer				// It will be 0xffffffd0

	.globl __romstrap_end

__romstrap_end:
.previous
