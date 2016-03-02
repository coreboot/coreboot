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

	SRAM_START(0xFF8C0000)
	BOOTBLOCK(0xFF8C2004, 32K - 4)
	PRERAM_CBFS_CACHE(0xFF8CA000, 4K)
	PRERAM_CBMEM_CONSOLE(0xFF8CB000, 4K)
	OVERLAP_VERSTAGE_ROMSTAGE(0xFF8CC000, 64K)
	VBOOT2_WORK(0XFF8DC000, 12K)
	TTB(0xFF8DF000, 32K)
	TIMESTAMP(0xFF8E7000, 1K)
	STACK(0xFF8E7400, 24K)
	SRAM_END(0xFF8F0000)
}
