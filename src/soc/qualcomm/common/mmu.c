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

#include <arch/mmu.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>

__weak void soc_mmu_dram_config_post_dram_init(void) { /* no-op */ }

void qc_mmu_dram_config_post_dram_init(void *ddr_base, size_t ddr_size)
{
	mmu_config_range((void *)ddr_base, ddr_size, CACHED_RAM);
	soc_mmu_dram_config_post_dram_init();
}
