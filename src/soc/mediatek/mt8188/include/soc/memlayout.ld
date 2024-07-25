/* SPDX-License-Identifier: GPL-2.0-only OR MIT */
#include <soc/memlayout.h>

SECTIONS
{
	/* MT8188 has 192KB SRAM in total. */
	SRAM_START(0x00100000)
	/* Regions that need to stay in SRAM. */
	TTB(0x00100000, 28K)
	DMA_COHERENT(0x00107000, 4K)
	STACK(0x00108000, 15K)
	WATCHDOG_TOMBSTONE(0x0010bc00, 4)
	/* EMPTY(0x0010bc04, 29K - 4) */
	/*
	 * MCUPM exchanges data with kernel driver using SRAM 0x00113000 ~
	 * 0x0011ffff. The address is hardcoded in MCUPM image.
	 */
	REGION(mcufw_reserved, 0x00113000, 52K, 4K)
	/* End of regions that need to stay in SRAM. */
	/* Regions can be moved to SRAM_L2C. */
	CBFS_MCACHE(0x00120000, 16k)
	VBOOT2_WORK(0x00124000, 12K)
	FMAP_CACHE(0x00127000, 2k)
	TPM_LOG(0x00127800, 2k)
	TIMESTAMP(0x00128000, 1k)
	/* End of regions that can also be moved to SRAM_L2C. */
	/* EMPTY(0x00128400, 31K) */
	SRAM_END(0x00130000)

	/*
	 * The L3 is 2MB in total. The bootROM has configured half of the L3 cache as SRAM
	 * (SRAM_L2C) so that's 1MB (and the rest to be cache, which is required so you
	 * can't reconfigure whole L3 as SRAM).
	 */
	SRAM_L2C_START(0x00200000)
	/*
	 * The bootROM needs 4K starting from SRAM_L2C_START so the bootblock starting address
	 * is fixed at SRAM_L2C_START + 4K, and the 4K can be reused after bootblock is started.
	 * To move the address, gen-bl-img.py also needs to be modified accordingly.
	 */
	BOOTBLOCK(0x00201000, 60K)
	/*
	 * The needed size can be obtained by:
	 *  aarch64-cros-linux-gnu-objdump -x dram.elf | grep memsz
	 * To move the address, dram.elf also needs to be modified accordingly.
	 */
	DRAM_INIT_CODE(0x00210000, 300K)
	OVERLAP_DECOMPRESSOR_VERSTAGE_ROMSTAGE(0x0025b000, 272K)
	PRERAM_CBFS_CACHE(0x0029f000, 48K)
	PRERAM_CBMEM_CONSOLE(0x002ab000, 340K)
	SRAM_L2C_END(0x00300000)

	DRAM_START(0x40000000)
	DRAM_DMA(0x40000000, 1M)
	POSTRAM_CBFS_CACHE(0x40100000, 2M)
	RAMSTAGE(0x40300000, 2M)

	BL31(0x54600000, 0x60000)
}
