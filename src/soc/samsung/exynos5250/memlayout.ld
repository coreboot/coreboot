/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

/*
 * Note: The BootROM loads the 8K BL1 at [0x2020000:0x2022000), so the bootblock
 * must be placed after that. After the handoff, the space can be reclaimed.
 */

SECTIONS
{
	SRAM_START(0x2020000)
	/* 13K hole, includes BL1 */
	BOOTBLOCK(0x2023400, 32K)
	/* 19K hole */
	ROMSTAGE(0x2030000, 128K)
	/* 32K hole */
	TTB(0x2058000, 16K)
	PRERAM_CBFS_CACHE(0x205C000, 68K)
	CBFS_MCACHE(0x206D000, 8K)
	FMAP_CACHE(0x206F000, 2K)
	TPM_LOG(0x206F800, 2K)
	VBOOT2_WORK(0x2070000, 12K)
	STACK(0x2074000, 16K)
	SRAM_END(0x2078000)

	DRAM_START(0x40000000)
	RAMSTAGE(0x40000000, 128K)
	POSTRAM_CBFS_CACHE(0x41000000, 8M)
	DMA_COHERENT(0x77300000, 1M)
}
