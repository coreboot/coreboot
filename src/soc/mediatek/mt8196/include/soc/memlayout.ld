/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <soc/memlayout.h>

SECTIONS
{
	/* MT8196 has 256KB SRAM. */
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
	/* EMPTY(0x00128400, 95K) */
	SRAM_END(0x00140000)

	/*
	 * The L3 (can be used as SRAM_L2C) currently using is 2MB.
	 * The BootROM has configured all cache as SRAM so we can't use them
	 * unless if we disable L2C and reconfigure.
	 */
	SRAM_L2C_START(0x02000000)
	#if ENV_ROMSTAGE
	/*
	 *  The needed size can be obtained by:
	 *  aarch64-cros-linux-gnu-objdump -x dram.elf | grep memsz
	 */
	DRAM_INIT_CODE(0x02000000, 600K)
	#else
	/* The beginning 4K of SRAM_L2C is reserved for BOOTROM until BOOTBLOCK is started. */
	BOOTBLOCK(0x02001000, 60K)
	#endif
	OVERLAP_DECOMPRESSOR_VERSTAGE_ROMSTAGE(0x02096000, 272K)
	PRERAM_CBFS_CACHE(0x020DA000, 48K)
	PRERAM_CBMEM_CONSOLE(0x020E6000, 340K)
	SRAM_L2C_END(0x02200000)

	DRAM_START(0x80000000)
	DRAM_DMA(0x80000000, 1M)
	POSTRAM_CBFS_CACHE(0x80100000, 2M)
	RAMSTAGE(0x80300000, 2M)

	BL31(0x94600000, 0x200000)
}
