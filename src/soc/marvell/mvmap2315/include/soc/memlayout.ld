/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#if	ENV_RAMSTAGE
	STACK(0x00080010, 8K)
#endif
	POSTRAM_CBFS_CACHE(0x1C0000, 256K)
	RAMSTAGE(0x00200000, 640K)
	TTB(0x00300000, 1024K)

	SRAM_START(0xE0000000)

	REGION(BDB, 0xE0000000, 8K, 1)
	REGION(BootROM_CB, 0xE0002000, 24K, 1)

#if	ENV_BOOTBLOCK
	STACK(0xE0008000, 2K)
#endif

#if	ENV_ROMSTAGE
	STACK(0xE0008800, 2K)
#endif

#if	ENV_ROMSTAGE || ENV_BOOTBLOCK

	TIMESTAMP(0xE0009000, 1K)
#endif

	PRERAM_CBFS_CACHE(0xE0009400, 256)
	REGION(bootblock_CB, 0xE0009500, 256, 1)
	PRERAM_CBMEM_CONSOLE(0xE0009600, 8K)
	REGION(fiq_stack, 0xE000B600, 2560, 1)
	BOOTBLOCK(0xE000C000, 16K)
	ROMSTAGE(0xE0010000, 64K)
	REGION(apmu, 0xE0020000, 64K, 1)
	SRAM_END(0xE0030000)
}
