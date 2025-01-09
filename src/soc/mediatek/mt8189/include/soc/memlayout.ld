/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <soc/memlayout.h>

SECTIONS
{
	/* MT8189 has 64KB SRAM. */
	SRAM_START(0x00100000)
	WATCHDOG_TOMBSTONE(0x00100030, 4)

	/*
	 * MCUPM uses the following regions to exchange data with kernel.
	 * The addresses are hardcoded in MCUPM image.
	 */
	REGION(dvfs1_reserved, 0x00100a00, 3K, 4)
	REGION(cpucooler_reserved, 0x00101600, 700, 4)
	REGION(mcdi_reserved, 0x00101d00, 2K, 4)
	REGION(thermal_reserved, 0x00102500, 1K, 4)
	REGION(dvfs2_reserved, 0x00108D68, 5K, 4)
	REGION(ptp1_reserved, 0x0010A168, 6808, 4)

	/*
	 * Since MCUPM uses most of the space, most regions are put in SRAM_L2C below.
	 */

	SRAM_END(0x00110000)

	/*
	 * The L3 (can be used as SRAM_L2C) currently using is 1MB.
	 * The BootROM has configured all cache as SRAM so we can't use them
	 * unless if we disable L2C and reconfigure.
	 */
	SRAM_L2C_START(0x02000000)
	#if ENV_ROMSTAGE
	/*
	 * The needed size can be obtained by:
	 *  aarch64-cros-linux-gnu-objdump -x dram.elf | grep memsz
	 * To move the address, dram.elf also needs to be modified accordingly.
	 */
	DRAM_INIT_CODE(0x02000000, 500K)
	#else
	/*
	 * The bootROM needs 4K starting from SRAM_L2C_START so the bootblock starting address
	 * is fixed at SRAM_L2C_START + 4K, and the 4K can be reused after bootblock is started.
	 * To move the address, gen-bl-img.py also needs to be modified accordingly.
	 */
	BOOTBLOCK(0x02001000, 70K)
	#endif

	OVERLAP_DECOMPRESSOR_VERSTAGE_ROMSTAGE(0x0207D000, 272K)
	PRERAM_CBFS_CACHE(0x020C1000, 48K)

	CBFS_MCACHE(0x020CD000, 16K)
	VBOOT2_WORK(0x020D1000, 12K)
	FMAP_CACHE(0x020D4000, 2K)
	TPM_LOG(0x020D4800, 2K)
	TTB(0x020D5000, 28K)
	DMA_COHERENT(0x020DC000, 4K)
	STACK(0x020DD000, 15K)
	TIMESTAMP(0x020E0C00, 1K)
	PRERAM_CBMEM_CONSOLE(0x020E1000, 124K)
	SRAM_L2C_END(0x02100000)

	DRAM_START(0x40000000)
	DRAM_DMA(0x40000000, 1M)
	POSTRAM_CBFS_CACHE(0x40100000, 2M)
	RAMSTAGE(0x40300000, 2M)
	FSP_RAMSTAGE_INIT_CODE(0x40500000, 2M)

	BL31(0x54600000, 0x60000)
}
