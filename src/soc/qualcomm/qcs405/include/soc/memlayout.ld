/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018, The Linux Foundation.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <memlayout.h>
#include <arch/header.ld>

/* SYSTEM_IMEM : 0x8600000 - 0x8607FFF */
#define SSRAM_START(addr) SYMBOL(ssram, addr)
#define SSRAM_END(addr) SYMBOL(essram, addr)

/* BOOT_IMEM   : 0x8C00000 - 0x8D80000 */
#define BSRAM_START(addr) SYMBOL(bsram, addr)
#define BSRAM_END(addr) SYMBOL(ebsram, addr)

SECTIONS
{
	SSRAM_START(0x8600000)
	SSRAM_END(0x8608000)

	BSRAM_START(0x8C00000)
	OVERLAP_VERSTAGE_ROMSTAGE(0x8C00000, 100K)
	REGION(fw_reserved2, 0x8C19000, 0x16000, 4096)
	BOOTBLOCK(0x8C2F000, 40K)
	TTB(0x8C39000, 56K)
	VBOOT2_WORK(0x8C47000, 12K)
	STACK(0x8C4B000, 16K)
	TIMESTAMP(0x8C4F000, 1K)
	PRERAM_CBMEM_CONSOLE(0x8C4F400, 32K)
	PRERAM_CBFS_CACHE(0x8C57400, 70K)
	FMAP_CACHE(0x8C68C00, 2K)
	REGION(bsram_unused, 0x8C69400, 0xA1C00, 0x100)
	BSRAM_END(0x8D80000)

	DRAM_START(0x80000000)
	/* DDR Carveout for BL31 usage */
	REGION(dram_reserved, 0x85000000, 0x5100000, 4096)
	POSTRAM_CBFS_CACHE(0x9F800000, 384K)
	RAMSTAGE(0x9F860000, 128K)
}
