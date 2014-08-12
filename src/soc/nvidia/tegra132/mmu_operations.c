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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <stdint.h>
#include <memrange.h>

#include <cbmem.h>
#include <console/console.h>

#include <arch/mmu.h>
#include "mmu_operations.h"
#include <soc/addressmap.h>

/* This structure keeps track of all the mmap memory ranges for t132 */
static struct memranges t132_mmap_ranges;

static void print_memranges(struct memranges *mmap_ranges)
{
	struct range_entry *mmap_entry;

	printk(BIOS_DEBUG,"printing mmap entries\n");

	memranges_each_entry(mmap_entry, mmap_ranges) {
		printk(BIOS_DEBUG,"0x%p 0x%p 0x%lx\n",
		       (void*)mmap_entry->begin,(void*)mmap_entry->end,mmap_entry->tag);
	}

}

static void tegra132_memrange_init(struct memranges *map)
{
	uint64_t start,end;
	const unsigned long devmem = MA_DEV | MA_NS | MA_RW;
	const unsigned long cachedmem = MA_MEM | MA_NS | MA_RW;

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
}

void __attribute__((weak)) mainboard_add_memory_ranges(struct memranges *map)
{
	/* Don't add any ranges by default. */
}

void tegra132_mmu_init(void)
{
	uint64_t *ttb_buffer = (uint64_t*)CONFIG_TTB_BUFFER;
	uint64_t ttb_size = (uint64_t)CONFIG_TTB_SIZE;
	struct memranges *map = &t132_mmap_ranges;

	tegra132_memrange_init(map);
	mainboard_add_memory_ranges(map);
	print_memranges(map);
	mmu_init(map,ttb_buffer,ttb_size);
	mmu_enable((uint64_t)ttb_buffer);
}
