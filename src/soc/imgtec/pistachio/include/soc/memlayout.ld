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
	/*
	 * All of DRAM (other than the DMA coherent area) is accessed through
	 * the identity mapping.
	 */
	DRAM_START(0x00000000)
	/* DMA coherent area: accessed via KSEG1. */
	DMA_COHERENT(0x00100000, 1M)
	POSTRAM_CBFS_CACHE(0x00200000, 128K)
	RAMSTAGE(0x00220000, 128K)

	/*
	 * GRAM becomes the SRAM.  Accessed through KSEG0 in the bootblock
	 * and then through the identity mapping in ROM stage.
	 */
	SRAM_START(0x1a000000)
	ROMSTAGE(0x1a004800, 36K)
	PRERAM_CBFS_CACHE(0x1a00d800, 74K)
	SRAM_END(0x1a020000)

	/* Bootblock executes out of KSEG0 and sets up the identity mapping. */
	BOOTBLOCK(0x9a000000, 18K)

	/*
	 * Let's use SRAM for stack and CBMEM console.  Always accessed
	 * through KSEG0.
	 */
	STACK(0x9b000000, 8K)
	PRERAM_CBMEM_CONSOLE(0x9b002000, 8K)
}
