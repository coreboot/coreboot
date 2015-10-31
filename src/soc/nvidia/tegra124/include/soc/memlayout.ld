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

/*
 * Note: The BootROM uses the address range [0x4000_0000:0x4000_E000) itself,
 * so the bootblock loading address must be placed after that. After the
 * handoff that area may be reclaimed for other uses, e.g. CBFS cache.
 */

SECTIONS
{
	SRAM_START(0x40000000)
	TTB(0x40000000, 16K + 32)
	PRERAM_CBMEM_CONSOLE(0x40004020, 8K - 32)
	PRERAM_CBFS_CACHE(0x40006000, 16K)
	VBOOT2_WORK(0x4000A000, 16K)
	STACK(0x4000E000, 8K)
	BOOTBLOCK(0x40010000, 24K)
	VERSTAGE(0x40016000, 72K)
	ROMSTAGE(0x40028000, 96K)
	SRAM_END(0x40040000)

	DRAM_START(0x80000000)
	POSTRAM_CBFS_CACHE(0x80100000, 1M)
	RAMSTAGE(0x80200000, 128K)
	DMA_COHERENT(0x90000000, 2M)
}
