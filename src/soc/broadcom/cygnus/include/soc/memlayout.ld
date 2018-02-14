/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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
	SRAM_START(0x02000000)
	REGION(reserved_for_system_status, 0x02000000, 4K, 4)
	TTB(0x02004000, 16K)		/* must be aligned to 16K */
	TTB_SUBTABLES(0x02008000, 1K)
	REGION(reserved_for_maskrom, 0x02009400, 4K, 4)
	BOOTBLOCK(0x0200A440, 18K)
	PRERAM_CBMEM_CONSOLE(0x0200F000, 2K)
	VBOOT2_WORK(0x0200f800, 16K)
	OVERLAP_VERSTAGE_ROMSTAGE(0x02013800, 122K)
	PRERAM_CBFS_CACHE(0x02032000, 1K)
	TIMESTAMP(0x02032400, 1K)
	STACK(0x02033000, 12K)
	REGION(reserved_for_secure_service_api, 0x0203F000, 4K, 4)
	SRAM_END(0x02040000)

	DRAM_START(0x60000000)
	RAMSTAGE(0x60000000, 128K)
	POSTRAM_CBFS_CACHE(0x60100000, 1M)
	DMA_COHERENT(0x60200000, 2M)
}
