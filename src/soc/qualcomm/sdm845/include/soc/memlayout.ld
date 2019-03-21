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
	OVERLAP_VERSTAGE_ROMSTAGE(0x14680000, 100K)
	DMA_COHERENT(0x14699000, 8K)
	REGION(qcsdi, 0x146AC000, 44K, 4K)
	SSRAM_END(0x146C0000)

	BSRAM_START(0x14800000)
	REGION(fw_reserved2, 0x14800000, 0x16000, 0x1000)
	BOOTBLOCK(0x14816000, 40K)
	TTB(0x14820000, 56K)
	VBOOT2_WORK(0x1482E000, 12K)
	STACK(0x14832000, 16K)
	TIMESTAMP(0x14836000, 1K)
	PRERAM_CBMEM_CONSOLE(0x14836400, 32K)
	PRERAM_CBFS_CACHE(0x1483E400, 70K)
	REGION(bsram_unused, 0x1484FC00, 0x9E300, 0x100)
	REGION(ddr_information, 0x148EDF00, 256, 256)
	REGION(limits_cfg, 0x148EE000, 4K, 4K)
	REGION(qclib_serial_log, 0x148EF000, 4K, 4K)
	REGION(ddr_training, 0x148F0000, 8K, 4K)
	REGION(qclib, 0x148F2000, 512K, 4K)
	REGION(dcb, 0x14972000, 16K, 4K)
	REGION(pmic, 0x14976000, 40K, 4K)
	BSRAM_END(0x14980000)

	DRAM_START(0x80000000)
	/* Various hardware/software subsystems make use of this area */
	REGION(dram_reserved, 0x85000000, 0x1A800000, 0x1000)
	POSTRAM_CBFS_CACHE(0x9F800000, 384K)
	RAMSTAGE(0x9F860000, 2M)
}
