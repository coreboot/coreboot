/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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

SECTIONS
{
	DRAM_START(0x00000000)
	POSTRAM_CBFS_CACHE(0x00100000, 1M)
	RAMSTAGE(0x00300000, 256K)
	DMA_COHERENT(0x10000000, 2M)
	FRAMEBUFFER(0x10200000, 8M)

	/* 8K of special SRAM in PMU power domain. */
	SYMBOL(pmu_sram, 0xFF3B0000)
	WATCHDOG_TOMBSTONE(0xFF3B1FFC, 4)
	SYMBOL(epmu_sram, 0xFF3B2000)

	SRAM_START(0xFF8C0000)
	PRERAM_CBMEM_CONSOLE(0xFF8C0000, 7K)
	TIMESTAMP(0xFF8C1C00, 1K)
	BOOTBLOCK(0xFF8C2004, 36K - 4)
	PRERAM_CBFS_CACHE(0xFF8CB000, 4K)
	OVERLAP_VERSTAGE_ROMSTAGE(0xFF8CC000, 92K)
	VBOOT2_WORK(0XFF8E3000, 12K)
	TTB(0xFF8E6000, 24K)
	STACK(0xFF8EC000, 16K)
	SRAM_END(0xFF8F0000)
}
