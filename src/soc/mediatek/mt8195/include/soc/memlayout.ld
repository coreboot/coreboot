/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <soc/memlayout.h>

SECTIONS
{
	SRAM_START(0x00100000)
	VBOOT2_WORK(0x00100000, 12K)
	TPM_LOG(0x00103000, 2K)
	FMAP_CACHE(0x00103800, 2K)
	WATCHDOG_TOMBSTONE(0x00104000, 4)
	EARLY_INIT(0x00104010, 128)
	CBFS_MCACHE(0x00107c00, 8K)
	TIMESTAMP(0x00109c00, 1K)
	STACK(0x0010a000, 12K)
	TTB(0x0010d000, 28K)
	DMA_COHERENT(0x00114000, 4K)
	/*
	 * MCUPM exchanges data with kernel driver using SRAM 0x00115000 ~
	 * 0x0011ffff. The address is hardcoded in MCUPM image and is unlikely
	 * to change.
	 */
	REGION(mcufw_reserved, 0x00115000, 44K, 4K)
	/* MT8195 has 192KB SRAM. */
	SRAM_END(0x00130000)

	/*
	 * The L3 (can be used as SRAM_L2C) is 2MB in total. However the BootROM
	 * has configured only half of L2/L3 cache as SRAM so we can't use them
	 * unless if we disable L2C and reconfigure.
	 */
	SRAM_L2C_START(0x00200000)
	/* 4K reserved for BOOTROM until BOOTBLOCK is started */
	BOOTBLOCK(0x00201000, 60K)
	/*
	 * The needed size can be obtained by:
	 *  aarch64-cros-linux-gnu-objdump -x dram.elf | grep memsz
	 */
	DRAM_INIT_CODE(0x00210000, 240K)
	OVERLAP_DECOMPRESSOR_VERSTAGE_ROMSTAGE(0x0024c000, 272K)
	PRERAM_CBFS_CACHE(0x00290000, 48K)
	PRERAM_CBMEM_CONSOLE(0x0029c000, 400K)
	SRAM_L2C_END(0x00300000)

	DRAM_START(0x40000000)
	DRAM_DMA(0x40000000, 1M)
	POSTRAM_CBFS_CACHE(0x40100000, 8M)
	RAMSTAGE(0x40900000, 2M)

	BL31(0x54600000, 0x60000)
}
