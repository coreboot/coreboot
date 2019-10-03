/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

/*
 * SRAM_L2C is the half part of L2 cache that we borrow to be used as SRAM.
 * It will be returned before starting the ramstage.
 * SRAM_L2C and SRAM can be cached, but only SRAM is DMA-able.
 */
#define SRAM_L2C_START(addr) SYMBOL(sram_l2c, addr)
#define SRAM_L2C_END(addr) SYMBOL(esram_l2c, addr)
#define DRAM_INIT_CODE(addr, size) \
	REGION(dram_init_code, addr, size, 4)

#define DRAM_DMA(addr, size) \
	REGION(dram_dma, addr, size, 4K) \
	_ = ASSERT(size % 4K == 0, \
		"DRAM DMA buffer should be multiple of smallest page size (4K)!");

SECTIONS
{
	SRAM_START(0x00100000)
	VBOOT2_WORK(0x00100000, 12K)
	TPM_TCPA_LOG(0x00103000, 2K)
	FMAP_CACHE(0x00103800, 2K)
	WATCHDOG_TOMBSTONE(0x00104000, 4)
	PRERAM_CBMEM_CONSOLE(0x00104004, 15K - 4)
	CBFS_MCACHE(0x00107c00, 8K)
	TIMESTAMP(0x00109c00, 1K)
	STACK(0x0010a000, 12K)
	TTB(0x0010d000, 28K)
	DMA_COHERENT(0x00114000, 4K)
	/*
	 * MCUPM exchanges data with kernel driver using SRAM 0x00115000 ~ 0x0011ffff.
	 * The address is hardcoded in MCUPM image and is unlikely to change.
	 */
	REGION(mcufw_reserved, 0x00115000, 44K, 4K)
	SRAM_END(0x00120000)

	SRAM_L2C_START(0x00200000)
	BOOTBLOCK(0x00201000, 64K)
	OVERLAP_DECOMPRESSOR_VERSTAGE_ROMSTAGE(0x00211000, 188K)
	DRAM_INIT_CODE(0x00240000, 208K)
	PRERAM_CBFS_CACHE(0x00274000, 48K)
	SRAM_L2C_END(0x00280000)

	DRAM_START(0x40000000)
	DRAM_DMA(0x40000000, 1M)
	POSTRAM_CBFS_CACHE(0x40100000, 2M)
	RAMSTAGE(0x40300000, 256K)

	BL31(0x54600000, 0x60000)
}
