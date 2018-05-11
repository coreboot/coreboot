/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018, The Linux Foundation.  All rights reserved.
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
#include <soc/symbols.h>

#define   CACHED_RAM (MA_MEM | MA_S | MA_RW)
#define UNCACHED_RAM (MA_MEM | MA_S | MA_RW | MA_MEM_NC)
#define      DEV_MEM (MA_DEV | MA_S | MA_RW)

void sdm845_mmu_init(void)
{
	mmu_init();

	mmu_config_range((void *)(4 * KiB), ((4UL * GiB) - (4 * KiB)), DEV_MEM);
	mmu_config_range((void *)_ssram, _ssram_size, CACHED_RAM);
	mmu_config_range((void *)_bsram, _bsram_size, CACHED_RAM);
	mmu_config_range((void *)_dma_coherent, _dma_coherent_size,
			 UNCACHED_RAM);

	mmu_enable();
}
