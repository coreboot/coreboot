/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#include <arch/io.h>
#include <arch/mmu.h>
#include <console/console.h>
#include <symbols.h>
#include <stdlib.h>
#include <stdint.h>
#include <soc/addressmap.h>
#include <soc/emi.h>
#include <soc/infracfg.h>
#include <soc/mcucfg.h>
#include <soc/mmu_operations.h>

void mt8173_mmu_init(void)
{
	mmu_init();

	/* Set 0x0 to the end of 2GB dram address as device memory */
	mmu_config_range((void *)0, (uintptr_t)_dram + 2U * GiB, DEV_MEM);

	/* SRAM is cached */
	mmu_config_range(_sram_l2c, _sram_l2c_size + _sram_size, CACHED_MEM);

	/* DMA is non-cached and is reserved for TPM & da9212 I2C DMA */
	mmu_config_range(_dma_coherent, _dma_coherent_size, UNCACHED_MEM);

	/* set ttb as secure */
	mmu_config_range(_ttb, _ttb_size, SECURE_MEM);

	mmu_enable();
}

void mt8173_mmu_after_dram(void)
{
	/* Map DRAM as cached now that it's up and running */
	mmu_config_range(_dram, (uintptr_t)sdram_size(), CACHED_MEM);

	/* Unmap L2C SRAM so it can be reclaimed by L2 cache */
	/* TODO: Implement true unmapping, and also use it for the zero-page! */
	mmu_config_range(_sram_l2c, _sram_l2c_size, DEV_MEM);

	mmu_config_range(_dram_dma, _dram_dma_size, UNCACHED_MEM);

	/* Careful: changing cache geometry while it's active is a bad idea! */
	mmu_disable();

	/* Return L2C SRAM back to L2 cache. Set it to 512KiB which is the max
	 * available L2 cache for A53 in MT8173. */
	write32(&mt8173_mcucfg->mp0_ca7l_cache_config, 3 << 8);
	/* turn off the l2c sram clock */
	write32(&mt8173_infracfg->infra_pdn0, L2C_SRAM_PDN);

	/* Reenable MMU with now enlarged L2 cache. Page tables still valid. */
	mmu_enable();
}
