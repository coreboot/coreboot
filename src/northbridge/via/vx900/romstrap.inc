/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer)
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
 * Copyright (C) 2011-2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* As extracted from the manufacturer's ROM, the romstrap table looks like:
 * .long 0x77886047 .long 0x00777777
 * .long 0x00000000 .long 0x00000000
 * .long 0x00888888 .long 0x00AA1111
 * .long 0x00000000 .long 0x00000000
 *
 * The vendor BIOS then adjusts some of these settings very early on. Instead of
 * adjusting those settings in code, we work them in the romstrap table.
 *
 */
/* This file constructs the ROM strap table for VX900 */

	.section ".romstrap", "a", @progbits

	.globl __romstrap_start
__romstrap_start:
tblpointer:
	.long 0x77886047
	.long 0x00777777
	.long 0x00000000
	.long 0x00000000
	.long 0x00888888
	.long 0x00AA1111
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
