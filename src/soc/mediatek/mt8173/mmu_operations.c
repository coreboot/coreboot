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
#include <symbols.h>
#include <soc/symbols.h>
#include <soc/infracfg.h>
#include <soc/mcucfg.h>
#include <soc/mmu_operations.h>

void mtk_soc_after_dram(void)
{
	mmu_config_range(_dram_dma, _dram_dma_size, NONSECURE_UNCACHED_MEM);
	mtk_mmu_disable_l2c_sram();
}

void mtk_soc_disable_l2c_sram(void)
{
	/* Return L2C SRAM back to L2 cache. Set it to 512KiB which is the max
	 * available L2 cache for A53 in MT8173. */
	write32(&mt8173_mcucfg->mp0_ca7l_cache_config, 3 << 8);
	/* turn off the l2c sram clock */
	write32(&mt8173_infracfg->infra_pdn0, L2C_SRAM_PDN);
}
