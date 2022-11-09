/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

/*
 * SRAM_L2C is the half part of L2 cache that we borrow it to be used as SRAM.
 * It will be returned before starting the ramstage.
 * SRAM_L2C and SRAM can be cached, but only SRAM is DMA-able.
 */
#define SRAM_L2C_START(addr) REGION_START(sram_l2c, addr)
#define SRAM_L2C_END(addr) REGION_END(sram_l2c, addr)

#define DRAM_DMA(addr, size) \
	REGION(dram_dma, addr, size, 4K) \
	_ = ASSERT(size % 4K == 0, \
		"DRAM DMA buffer should be multiple of smallest page size (4K)!");

SECTIONS
{
	SRAM_L2C_START(0x000C0000)
	BOOTBLOCK(0x000C1000, 85K)
	VERSTAGE(0x000D7000, 114K)
	SRAM_L2C_END(0x00100000)

	SRAM_START(0x00100000)
	VBOOT2_WORK(0x00100000, 12K)
	TPM_LOG(0x00103000, 2K)
	FMAP_CACHE(0x00103800, 2K)
	PRERAM_CBMEM_CONSOLE(0x00104000, 12K)
	WATCHDOG_TOMBSTONE(0x00107000, 4)
	PRERAM_CBFS_CACHE(0x00107004, 8K - 4)
	CBFS_MCACHE(0x00109000, 8K)
	TIMESTAMP(0x0010B000, 4K)
	ROMSTAGE(0x0010C000, 92K)
	STACK(0x00124000, 16K)
	TTB(0x00128000, 28K)
	DMA_COHERENT(0x0012F000, 4K)
	SRAM_END(0x00130000)

	DRAM_START(0x40000000)
	DRAM_DMA(0x40000000, 1M)
	POSTRAM_CBFS_CACHE(0x40100000, 1M)
	RAMSTAGE(0x40200000, 256K)
}
