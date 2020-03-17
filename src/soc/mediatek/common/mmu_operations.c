/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/mmu.h>
#include <symbols.h>
#include <soc/emi.h>
#include <soc/mmu_operations.h>

__weak void mtk_soc_after_dram(void) { /* do nothing */ }

void mtk_mmu_init(void)
{
	mmu_init();

	/*
	 * Set 0x0 to 4GB address as device memory. We want to config IO_PHYS
	 * address to DEV_MEM, and map a proper range of dram for the memory
	 * test during calibration.
	 */
	mmu_config_range((void *)0, (uintptr_t)4U * GiB, DEV_MEM);

	/* SRAM is cached */
	mmu_config_range(_sram, REGION_SIZE(sram), SECURE_CACHED_MEM);

	/* L2C SRAM is cached */
	mmu_config_range(_sram_l2c, REGION_SIZE(sram_l2c), SECURE_CACHED_MEM);

	/* DMA is non-cached and is reserved for TPM & da9212 I2C DMA */
	mmu_config_range(_dma_coherent, REGION_SIZE(dma_coherent),
			 SECURE_UNCACHED_MEM);

	mmu_enable();
}

void mtk_mmu_after_dram(void)
{
	/* Map DRAM as cached now that it's up and running */
	mmu_config_range(_dram, (uintptr_t)sdram_size(), NONSECURE_CACHED_MEM);

	mtk_soc_after_dram();
}

void mtk_mmu_disable_l2c_sram(void)
{
	/* Unmap L2C SRAM so it can be reclaimed by L2 cache */
	/* TODO: Implement true unmapping, and also use it for the zero-page! */
	mmu_config_range(_sram_l2c, REGION_SIZE(sram_l2c), DEV_MEM);

	/* Careful: changing cache geometry while it's active is a bad idea! */
	mmu_disable();

	mtk_soc_disable_l2c_sram();

	/* Reenable MMU with now enlarged L2 cache. Page tables still valid. */
	mmu_enable();
}
