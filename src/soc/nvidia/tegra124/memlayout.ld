/* SPDX-License-Identifier: GPL-2.0-only */

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
	PRERAM_CBMEM_CONSOLE(0x40004020, 6K - 32)
	FMAP_CACHE(0x40005800, 2K)
	CBFS_MCACHE(0x40006000, 8K)
	PRERAM_CBFS_CACHE(0x40008000, 6K)
	VBOOT2_WORK(0x40009800, 12K)
	TPM_LOG(0x4000D800, 2K)
	STACK(0x4000E000, 8K)
	BOOTBLOCK(0x40010000, 32K)
	VERSTAGE(0x40018000, 70K)
	ROMSTAGE(0x40029800, 89K)
	TIMESTAMP(0x4003FC00, 1K)
	SRAM_END(0x40040000)

	DRAM_START(0x80000000)
	POSTRAM_CBFS_CACHE(0x80100000, 1M)
	RAMSTAGE(0x80200000, 128K)
	DMA_COHERENT(0x90000000, 2M)
}
