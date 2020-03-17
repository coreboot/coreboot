/*
 * This file is part of the coreboot project.
 *
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

void qcs405_mmu_init(void)
{
	mmu_init();

	mmu_config_range((void *)(4 * KiB), ((4UL * GiB) - (4 * KiB)),
			MA_DEV | MA_S | MA_RW);
	mmu_config_range((void *)_ssram, REGION_SIZE(ssram), MA_MEM | MA_S | MA_RW);
	mmu_config_range((void *)_bsram, REGION_SIZE(bsram), MA_MEM | MA_S | MA_RW);

	mmu_enable();
}
