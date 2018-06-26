/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

/*
 * SRAM_L2C is the half part of L2 cache that we borrow to be used as SRAM.
 * It will be returned before starting the ramstage.
 * SRAM_L2C and SRAM can be cached, but only SRAM is DMA-able.
 */
#define SRAM_L2C_START(addr) SYMBOL(sram_l2c, addr)
#define SRAM_L2C_END(addr) SYMBOL(esram_l2c, addr)

SECTIONS
{
	SRAM_START(0x00100000)
	VBOOT2_WORK(0x00100000, 12K)
	PRERAM_CBMEM_CONSOLE(0x00103000, 16K)
	WATCHDOG_TOMBSTONE(0x00107000, 4)
	PRERAM_CBFS_CACHE(0x00107004, 16K - 4)
	TIMESTAMP(0x0010B000, 4K)
	STACK(0x0010C000, 16K)
	TTB(0x00110000, 28K)
	DMA_COHERENT(0x00117000, 4K)
	SRAM_END(0x00120000)

	SRAM_L2C_START(0x00200000)
	OVERLAP_DECOMPRESSOR_ROMSTAGE(0x000201000, 92K)
	BOOTBLOCK(0x00227000, 89K)
	VERSTAGE(0x0023E000, 114K)
	SRAM_L2C_END(0x00280000)

	DRAM_START(0x40000000)
	POSTRAM_CBFS_CACHE(0x40000000, 2M)
	RAMSTAGE(0x40200000, 256K)
}
