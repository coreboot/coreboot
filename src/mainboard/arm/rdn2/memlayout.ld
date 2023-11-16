/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <memlayout.h>
#include <arch/header.ld>

/*
 * Memory map for Neoverse N2
 * See Technical Reference: Table 5-2: FVP board peripherals
 */
SECTIONS
{
	REGION(flash, 0x8000000, CONFIG_ROM_SIZE, 8)

	REGION(dev_mem, 0x0c010000, 2M, 8)
	DRAM_START(0x80000000)
	BOOTBLOCK(0xe0000000, 64K)
	STACK(0xe0010000, 64K)
	CBFS_MCACHE(0xe0020000, 8K)
	FMAP_CACHE(0xe0022000 , 4K)
	TIMESTAMP(0xe0023000, 4K)
	ROMSTAGE(0xe0024000, 128K)
	TTB(0xe0060000, 128K)
	RAMSTAGE(0xe00a0000, 16M)
	REGION(fdt_pointer, 0xe10a0000, ARCH_POINTER_ALIGN_SIZE, ARCH_POINTER_ALIGN_SIZE)

	POSTRAM_CBFS_CACHE(0xe11f0000, 1M)
}
