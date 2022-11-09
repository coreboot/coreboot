/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

/*
 * Note: The BootROM uses the address range [0x4000_0000:0x4000_E000) itself,
 * so the bootblock loading address must be placed after that. After the
 * handoff that area may be reclaimed for other uses, e.g. CBFS cache.
 * TODO: Did this change on Tegra210? What's the new valid range?
 */

SECTIONS
{
	SRAM_START(0x40000000)
	PRERAM_CBMEM_CONSOLE(0x40000000, 2K)
	FMAP_CACHE(0x40000800, 2K)
	PRERAM_CBFS_CACHE(0x40001000, 20K)
	CBFS_MCACHE(0x40006000, 8K)
	VBOOT2_WORK(0x40008000, 12K)
	TPM_LOG(0x4000B000, 2K)
#if ENV_ARM64
	STACK(0x4000B800, 3K)
#else	/* AVP gets a separate stack to avoid any chance of handoff races. */
	STACK(0x4000C400, 3K)
#endif
	TIMESTAMP(0x4000D000, 2K)
	BOOTBLOCK(0x4000D800, 42K)
	OVERLAP_VERSTAGE_ROMSTAGE(0x40018000, 160K)
	SRAM_END(0x40040000)

	DRAM_START(0x80000000)
	POSTRAM_CBFS_CACHE(0x80100000, 1M)
	RAMSTAGE(0x80200000, 256K)
	TTB(0x100000000 - CONFIG_TTB_SIZE_MB * 1M, CONFIG_TTB_SIZE_MB * 1M)
}
