/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
#include <assert.h>
#include <soc/addressmap.h>
#include <soc/mmu_operations.h>
#include <stdlib.h>
#include <stdint.h>
#include <symbols.h>

static void tegra132_mmu_config(void)
{
	uint64_t start,end;
	const unsigned long devmem = MA_DEV | MA_S | MA_RW;
	const unsigned long cachedmem = MA_MEM | MA_NS | MA_RW;
	const unsigned long secure_mem = MA_MEM | MA_S | MA_RW;
	uintptr_t tz_base_mib;
	size_t tz_size_mib;

	memory_in_range_below_4gb(&start,&end);

	/* Device memory below DRAM */
	mmu_config_range((void *)0, start * MiB, devmem);

	/* DRAM */
	mmu_config_range((void *)(start * MiB), (end-start) * MiB, cachedmem);

	memory_in_range_above_4gb(&start,&end);

	mmu_config_range((void *)(start * MiB), (end-start) * MiB, cachedmem);

	/* SRAM */
	mmu_config_range(_sram, _sram_size, cachedmem);

	/* Add TZ carveout. */
	carveout_range(CARVEOUT_TZ, &tz_base_mib, &tz_size_mib);

	mmu_config_range((void *)(tz_base_mib * MiB),
			 tz_size_mib * MiB, secure_mem);

	/* Ensure page tables are at the base of the trust zone region. */
	assert((uintptr_t)_ttb == tz_base_mib * MiB &&
	       _ttb_size <= tz_size_mib * MiB);
}

void tegra132_mmu_init(void)
{
	mmu_init();
	tegra132_mmu_config();
	mmu_enable();
}
