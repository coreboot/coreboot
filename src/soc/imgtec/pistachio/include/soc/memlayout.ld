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
	CBFS_CACHE(0x9a00d000, 76K)
	SRAM_END(0x9a020000)

	/* Let's use SRAM for stack and CBMEM console. */
	STACK(0x9b000000, 8K)
	PRERAM_CBMEM_CONSOLE(0x9b002000, 8K)

	/* DMA coherent area: end of available DRAM, uncached */
	DMA_COHERENT(0xAFF00000, 1M)
}
