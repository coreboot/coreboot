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

/* SYSTEM_IMEM : 0x14680000 - 0x146C0000 */
#define SSRAM_START(addr) SYMBOL(ssram, addr)
#define SSRAM_END(addr) SYMBOL(essram, addr)

/* BOOT_IMEM   : 0x14800000 - 0x14980000 */
#define BSRAM_START(addr) SYMBOL(bsram, addr)
#define BSRAM_END(addr) SYMBOL(ebsram, addr)

SECTIONS
{
	SSRAM_START(0x14680000)
	OVERLAP_VERSTAGE_ROMSTAGE(0x14680000, 95K)
	REGION(fw_reserved1, 0x146A0000, 0x20000, 4096)
	SSRAM_END(0x146C0000)

	BSRAM_START(0x14800000)
	REGION(fw_reserved2, 0x14800000, 0x16000, 4096)
	BOOTBLOCK(0x14816000, 32K)
	TTB(0x1481E000, 64K)
	VBOOT2_WORK(0x1482E000, 16K)
	STACK(0x14832000, 16K)
	TIMESTAMP(0x14836000, 1K)
	PRERAM_CBMEM_CONSOLE(0x14836400, 32K)
	PRERAM_CBFS_CACHE(0x1483E400, 70K)
	REGION(bsram_unused, 0x1484FC00, 0xA2400, 0x100)
	REGION(qclib, 0x148F2000, 0x80000, 4096)
	REGION(dcb, 0x14972000, 0x4000, 4096)
	REGION(pmic, 0x14976000, 0xA000, 4096)
	BSRAM_END(0x14980000)

	DRAM_START(0x90000000)
	POSTRAM_CBFS_CACHE(0x90000000, 384K)
	RAMSTAGE(0x90800000, 128K)
}
