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
#include <vendorcode/google/chromeos/vboot2/memlayout.h>

#include <arch/header.ld>

SECTIONS
{
	SRAM_START(0x2A000000)
	/* This includes bootblock image, can be reused after bootblock starts */
/*	UBER_SBL(0x2A000000, 48K) */
/*	DDR(0x2A000000, 48K) */
	BOOTBLOCK(0x2A00C000, 24K)
	OVERLAP_VERSTAGE_ROMSTAGE(0x2A012000, 64K)
	VBOOT2_WORK(0x2A022000, 16K)
	PRERAM_CBMEM_CONSOLE(0x2A026000, 32K)
/*	0x2e000..0x3F000  68 KB free */

/* Keep the below area reserved at all times, it is used by various QCA
   components as shared data
	QCA_SHARED_RAM(2A03F000, 4K)
*/
	STACK(0x2A040000, 16K)
	CBFS_CACHE(0x2A044000, 96K)
	TTB(0x2A05C000, 16K)
	SRAM_END(0x2A060000)

	DRAM_START(0x40000000)
	RAMSTAGE(0x40640000, 128K)
	DMA_COHERENT(0x5A000000, 2M)
}
