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
#include <memrange.h>
#include <soc/addressmap.h>
#include <soc/mmu_operations.h>
#include <stdlib.h>
#include <stdint.h>

/* This structure keeps track of all the mmap memory ranges for t132 */
static struct memranges t132_mmap_ranges;

static void tegra132_memrange_init(struct memranges *map)
{
	uint64_t start,end;
	const unsigned long devmem = MA_DEV | MA_S | MA_RW;
	const unsigned long cachedmem = MA_MEM | MA_NS | MA_RW;
	const unsigned long secure_mem = MA_MEM | MA_S | MA_RW;
	uintptr_t tz_base_mib;
	size_t tz_size_mib;

	memranges_init_empty(map);

	memory_in_range_below_4gb(&start,&end);

	/* Device memory below DRAM */
	memranges_insert(map, 0, start * MiB, devmem);

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

void tegra132_mmu_init(void)
{
	uintptr_t tz_base_mib;
	size_t tz_size_mib;
	size_t ttb_size_mib;
	struct memranges *map = &t132_mmap_ranges;

	tegra132_memrange_init(map);
	mainboard_add_memory_ranges(map);
	/* Place page tables at the base of the trust zone region. */
	carveout_range(CARVEOUT_TZ, &tz_base_mib, &tz_size_mib);
	tz_base_mib *= MiB;
	ttb_size_mib = TTB_SIZE * MiB;
	mmu_init(map, (void *)tz_base_mib, ttb_size_mib);
	mmu_enable();
}
