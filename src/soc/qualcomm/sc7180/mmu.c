/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018-2019, The Linux Foundation.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <symbols.h>
#include <arch/mmu.h>
#include <arch/cache.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/symbols.h>

void sc7180_mmu_init(void)
{
	mmu_init();

	mmu_config_range((void *)(4 * KiB), ((4UL * GiB) - (4 * KiB)), DEV_MEM);
	mmu_config_range((void *)_ssram, REGION_SIZE(ssram), CACHED_RAM);
	mmu_config_range((void *)_bsram, REGION_SIZE(bsram), CACHED_RAM);
	mmu_config_range((void *)_dma_coherent, REGION_SIZE(dma_coherent),
			 UNCACHED_RAM);

	mmu_enable();
}

void soc_mmu_dram_config_post_dram_init(void)
{
	mmu_config_range((void *)_aop, REGION_SIZE(aop), CACHED_RAM);
}
