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
	SRAM_START(0x00100000)
	VBOOT2_WORK(0x00100000, 12K)
	STACK(0x00103000, 8K)
	TTB(0x00105000, 28K)
	DMA_COHERENT(0x0010C000, 4K)
	TPM_TCPA_LOG(0x0010D000, 2K)
	FMAP_CACHE(0x0010D800, 2K)
	WATCHDOG_TOMBSTONE(0x0010E000, 4)
	CBFS_MCACHE(0x0010E004, 7K - 4)
	TIMESTAMP(0x0010FC00, 1K)
	/* MT8186 has 64KB SRAM. */
	SRAM_END(0x00110000)

	/*
	 * The L3 (can be used as SRAM_L2C) is 1MB in total. However the BootROM
	 * has configured only half of L2/L3 cache as SRAM and the rest for cache
	 * so we can't use them unless if we disable L2C and reconfigure (be aware
	 * we can't configure whole L3 to SRAM without any cache).
	 */
	SRAM_L2C_START(0x00200000)
	/* 4K reserved for BOOTROM until BOOTBLOCK is started */
	BOOTBLOCK(0x00201000, 60K)
	/*
	 * The needed size can be obtained by:
	 *  aarch64-cros-linux-gnu-objdump -x dram.elf | grep memsz
	 */
	DRAM_INIT_CODE(0x00210000, 196K)
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
