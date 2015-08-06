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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/mmu.h>
#include <assert.h>
#include <memrange.h>
#include <soc/addressmap.h>
#include <soc/mmu_operations.h>
#include <stdlib.h>
#include <stdint.h>

/* This structure keeps track of all the mmap memory ranges for t210 */
static struct memranges t210_mmap_ranges;

static void tegra210_memrange_init(struct memranges *map)
{
	uint64_t start,end;
	const unsigned long devmem = MA_DEV | MA_S | MA_RW;
	const unsigned long cachedmem = MA_MEM | MA_NS | MA_RW;
	const unsigned long secure_mem = MA_MEM | MA_S | MA_RW;
	uintptr_t tz_base_mib;
	size_t tz_size_mib;

	print_carveouts();

	memranges_init_empty(map);

	memory_in_range_below_4gb(&start,&end);

	/* Device memory below DRAM */
	memranges_insert(map, TEGRA_ARM_LOWEST_PERIPH, start * MiB, devmem);

	/* DRAM */
	memranges_insert(map, start * MiB, (end-start) * MiB, cachedmem);

	memory_in_range_above_4gb(&start,&end);

	memranges_insert(map, start * MiB, (end-start) * MiB, cachedmem);

	/* SRAM */
	memranges_insert(map, TEGRA_SRAM_BASE, TEGRA_SRAM_SIZE, cachedmem);

	/* Add TZ carveout. */
	carveout_range(CARVEOUT_TZ, &tz_base_mib, &tz_size_mib);
	memranges_insert(map, tz_base_mib * MiB, tz_size_mib * MiB, secure_mem);
}

void __attribute__((weak)) mainboard_add_memory_ranges(struct memranges *map)
{
	/* Don't add any ranges by default. */
}

void tegra210_mmu_init(void)
{
	uintptr_t tz_base_mib;
	size_t tz_size_mib;
	uintptr_t ttb_base_mib;
	size_t ttb_size_mib;
	struct memranges *map = &t210_mmap_ranges;

	tegra210_memrange_init(map);
	mainboard_add_memory_ranges(map);
	/*
	 * Place page tables at the end of the trust zone region.
	 * TZDRAM layout is as follows:
	 *
	 * +--------------------------+ <----+DRAM_END
	 * |                          |
	 * |                          |
	 * |                          |
	 * +--------------------------+ <----+0x100000000
	 * |                          |
	 * |   coreboot page tables   |
	 * +--------------------------+
	 * |                          |
	 * |        BL32              |
	 * +--------------------------+
	 * |                          |
	 * |        BL31              |
	 * +--------------------------+ <----+TZDRAM_BASE
	 * |                          |
	 * |                          |
	 * |                          |
	 * |                          |
	 * +--------------------------+ <----+DRAM_BASE
	 *
	 */
	carveout_range(CARVEOUT_TZ, &tz_base_mib, &tz_size_mib);

	assert(tz_size_mib > CONFIG_TTB_SIZE_MB);
	ttb_base_mib = (tz_base_mib + tz_size_mib - CONFIG_TTB_SIZE_MB) * MiB;

	ttb_size_mib = CONFIG_TTB_SIZE_MB * MiB;
	mmu_init(map, (void *)ttb_base_mib, ttb_size_mib);
	mmu_enable();
}
