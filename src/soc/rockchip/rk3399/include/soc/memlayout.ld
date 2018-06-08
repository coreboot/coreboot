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

	/* 8K of special SRAM in PMU power domain. */
	SYMBOL(pmu_sram, 0xFF3B0000)
	WATCHDOG_TOMBSTONE(0xFF3B1FFC, 4)
	SYMBOL(epmu_sram, 0xFF3B2000)

	SRAM_START(0xFF8C0000)
	PRERAM_CBFS_CACHE(0xFF8C0000, 7K)
	TIMESTAMP(0xFF8C1C00, 1K)
	/* 0xFF8C2004 is the entry point address the masked ROM will jump to. */
	OVERLAP_DECOMPRESSOR_VERSTAGE_ROMSTAGE(0xFF8C2004, 88K - 4)
	BOOTBLOCK(0xFF8D8000, 40K)
	VBOOT2_WORK(0XFF8E2000, 12K)
	TTB(0xFF8E5000, 24K)
	PRERAM_CBMEM_CONSOLE(0xFF8EB000, 8K)
	STACK(0xFF8ED000, 12K)
	SRAM_END(0xFF8F0000)
}
