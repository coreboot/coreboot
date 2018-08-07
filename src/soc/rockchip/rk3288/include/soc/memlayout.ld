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
 */

#include <memlayout.h>

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
	FRAMEBUFFER(0x10800000, 8M)

	SRAM_START(0xFF700000)
	TTB(0xFF700000, 16K)
	BOOTBLOCK(0xFF704004, 20K - 4)
	PRERAM_CBMEM_CONSOLE(0xFF709000, 3K)
	VBOOT2_WORK(0xFF709C00, 12K)
	OVERLAP_VERSTAGE_ROMSTAGE(0xFF70CC00, 41K)
	PRERAM_CBFS_CACHE(0xFF717000, 1K)
	TIMESTAMP(0xFF717400, 0x180)
	STACK(0xFF717580, 3K - 0x180)
	SRAM_END(0xFF718000)

	/* 4K of special SRAM in PMU power domain.
	 * Careful: only supports 32-bit wide write accesses! */
	SYMBOL(pmu_sram, 0xFF720000)
	TTB_SUBTABLES(0xFF720800, 1K)
	WATCHDOG_TOMBSTONE(0xFF720FFC, 4)
	SYMBOL(epmu_sram, 0xFF721000)
}
