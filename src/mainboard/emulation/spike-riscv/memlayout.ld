/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

#define START 0x80000000

SECTIONS
{
	DRAM_START(START)
	BOOTBLOCK(START, 64K)
	STACK(START + 8M, 4K)
	FMAP_CACHE(START + 8M + 4K, 2K)
	CBFS_MCACHE(START + 8M + 6K, 8K)
	/* hole at (START + 8M + 14K, 50K) */
	ROMSTAGE(START + 8M + 64K, 128K)
	PRERAM_CBMEM_CONSOLE(START + 8M + 192k, 8K)
	RAMSTAGE(START + 8M + 200K, 256K)
}
