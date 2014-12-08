/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <memlayout.h>

#include <arch/header.ld>

SECTIONS
{
	DRAM_START(0x80000000)
	RAMSTAGE(0x80000000, 128K)

	/* GRAM becomes the SRAM. */
	SRAM_START(0x9a000000)
	BOOTBLOCK(0x9a000000, 16K)
	ROMSTAGE(0x9a004000, 36K)
	STACK(0x9a01c000, 8K)
	PRERAM_CBMEM_CONSOLE(0x9a01e000, 8K)
	SRAM_END(0x9a020000)

	/* Let's use SRAM for CBFS cache. */
	CBFS_CACHE(0x9b000000, 64K)
}
