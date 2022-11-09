/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

/*
 * SRAM_L2C is the half part of L2 cache that we borrow to be used as SRAM.
 * It will be returned before starting the ramstage.
 * SRAM_L2C and SRAM can be cached, but only SRAM is DMA-able.
 */
#define SRAM_L2C_START(addr) REGION_START(sram_l2c, addr)
#define SRAM_L2C_END(addr) REGION_END(sram_l2c, addr)
#define DRAM_INIT_CODE(addr, size) \
	REGION(dram_init_code, addr, size, 64K)

#define DRAM_DMA(addr, size) \
	REGION(dram_dma, addr, size, 4K) \
	_ = ASSERT(size % 4K == 0, \
		"DRAM DMA buffer should be multiple of smallest page size (4K)!");

SECTIONS
{
	/* MT8186 has 64KB SRAM in total. */
	SRAM_START(0x00100000)
	/* Regions that need to stay in SRAM. */
	TTB(0x00100000, 28K)
	DMA_COHERENT(0x00107000, 4K)
	STACK(0x00108000, 10K)
	WATCHDOG_TOMBSTONE(0x0010a800, 4)
	/* EMPTY(0x0010a804, 1K - 4) */
	/* Regions that can also be moved to SRAM_L2C. */
	TIMESTAMP(0x0010ac00, 1K)
	TPM_LOG(0x0010b000, 2K)
	FMAP_CACHE(0x0010b800, 2K)
	CBFS_MCACHE(0x0010c000, 16K)
	SRAM_END(0x00110000)

	/*
	 * The L3 (can be used as SRAM_L2C) is 1MB in total. However the bootROM has configured
	 * only half of L2/L3 cache as SRAM and the rest for cache so we can't use them unless
	 * if we disable L2C and reconfigure (be aware we can't configure whole L3 to SRAM
	 * without any cache).
	 */
	SRAM_L2C_START(0x00200000)
	/*
	 * The bootROM needs 4K starting from SRAM_L2C_START so the bootblock starting address
	 * is fixed at SRAM_L2C_START + 4K, and the 4K can be reused after bootblock is started.
	 */
	BOOTBLOCK(0x00201000, 60K)
	/*
	 * The needed size can be obtained by:
	 *  aarch64-cros-linux-gnu-objdump -x dram.elf | grep memsz
	 */
	DRAM_INIT_CODE(0x00210000, 184K)
	VBOOT2_WORK(0x0023e000, 12K)
	OVERLAP_DECOMPRESSOR_VERSTAGE_ROMSTAGE(0x00241000, 140K)
	PRERAM_CBFS_CACHE(0x00264000, 48K)
	PRERAM_CBMEM_CONSOLE(0x00270000, 64K)
	SRAM_L2C_END(0x00280000)

	DRAM_START(0x40000000)
	DRAM_DMA(0x40000000, 1M)
	POSTRAM_CBFS_CACHE(0x40100000, 2M)
	RAMSTAGE(0x40300000, 256K)

	BL31(0x54600000, 0x60000)
}
