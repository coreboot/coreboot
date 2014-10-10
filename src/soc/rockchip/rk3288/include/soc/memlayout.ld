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

/* Note: The BootROM will jump to 0xFF704004 after loading bootblock,
 * so the bootblock loading address must be at 0xFF704004.
 */
SECTIONS
{
	DRAM_START(0x00000000)
	RAMSTAGE(0x00200000, 128K)
	POSTRAM_CBFS_CACHE(0x01000000, 1M)
	DMA_COHERENT(0x10000000, 2M)

	SRAM_START(0xFF700000)
	TTB(0xFF700000, 16K)
	BOOTBLOCK(0xFF704004, 15K - 4)
	TTB_SUBTABLES(0xFF707c00, 1K)
	VBOOT2_WORK(0xFF708000, 16K)
	OVERLAP_VERSTAGE_ROMSTAGE(0xFF70C000, 40K)
	PRERAM_CBFS_CACHE(0xFF716000, 4K)
	STACK(0xFF717000, 4K)
	SRAM_END(0xFF718000)

	/* 4K of special SRAM in PMU power domain. Careful: only supports 32-bit
	 * wide write accesses! Only use with MMU and writeback mapping. */
	SYMBOL(pmu_sram, 0xFF720000)
	SYMBOL(epmu_sram, 0xFF721000)
}
