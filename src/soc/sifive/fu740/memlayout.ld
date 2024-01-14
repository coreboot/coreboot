/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>
#include <soc/addressmap.h>

#include <arch/header.ld>

SECTIONS
{
	SRAM_START(FU740_L2LIM)
	/* The 64K size is not allocated. It's just for basic size checking. */
	BOOTBLOCK(FU740_L2LIM, 64K)
	CAR_STACK(FU740_L2LIM + 64K, 12K) // considering each harts stack is a pagesize (4K), we can potentially have up to 16 harts regarding stack space //TODO reduce it, since fu740 doesn't even have/support 16 harts
	PRERAM_CBMEM_CONSOLE(FU740_L2LIM + 76K, 8K)
	CBFS_MCACHE(FU740_L2LIM + 84K, 8K)
	FMAP_CACHE(FU740_L2LIM + 92K, 2K)
	PRERAM_CBFS_CACHE(FU740_L2LIM + 94K, 128K)
	SRAM_END(FU740_L2LIM + 2M)

	DRAM_START(FU740_DRAM)
	OPENSBI(FU740_DRAM, 256K)
	/* The 256K size is not allocated. It's just for basic size checking. */
	RAMSTAGE(FU740_DRAM + 256K, 256K)
	MEM_STACK(FU740_DRAM + 512K, 20K)
	POSTRAM_CBFS_CACHE(FU740_DRAM + 532K, 32M - 532K)
}
