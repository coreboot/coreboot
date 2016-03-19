/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <arch/cache.h>
#include <arch/cpu.h>
#include <arch/exception.h>
#include <arch/io.h>
#include <arch/mmu.h>
#include <cbfs.h>
#include <console/console.h>
#include <delay.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <symbols.h>
#include <soc/mmu_operations.h>

static const uint64_t dram_size = (uint64_t)CONFIG_DRAM_SIZE_MB * MiB;

void main(void)
{
	console_init();
	exception_init();

	/*TODO: need implement sdram init */

	mmu_config_range((void *)0, (uintptr_t)dram_size, CACHED_MEM);
	mmu_config_range(_dma_coherent, _dma_coherent_size, UNCACHED_MEM);
	cbmem_initialize_empty();
	run_ramstage();
}
