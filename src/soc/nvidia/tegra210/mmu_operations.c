/*
 * This file is part of the coreboot project.
 *
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
#include <symbols.h>
#include <types.h>

static void tegra210_mmu_config(void)
{
	uintptr_t start, end;
	const unsigned long devmem = MA_DEV | MA_S | MA_RW;
	const unsigned long cachedmem = MA_MEM | MA_NS | MA_RW;
	const unsigned long secure_mem = MA_MEM | MA_S | MA_RW;
	uintptr_t tz_base_mib;
	size_t tz_size_mib;

	print_carveouts();

	memory_in_range_below_4gb(&start,&end);

	/* Device memory below DRAM */
	mmu_config_range((void *)TEGRA_ARM_LOWEST_PERIPH, start * MiB, devmem);

	/* DRAM */
	mmu_config_range((void *)(start * MiB), (end-start) * MiB, cachedmem);

	memory_in_range_above_4gb(&start,&end);

	mmu_config_range((void *)(start * MiB), (end-start) * MiB, cachedmem);

	/* SRAM */
	mmu_config_range(_sram, REGION_SIZE(sram), cachedmem);

	/* Add TZ carveout. */
	carveout_range(CARVEOUT_TZ, &tz_base_mib, &tz_size_mib);

	mmu_config_range((void *)(tz_base_mib * MiB),
			 tz_size_mib * MiB, secure_mem);
}

void tegra210_mmu_init(void)
{
	uintptr_t tz_base_mib;
	size_t tz_size_mib;

	mmu_init();
	tegra210_mmu_config();
	/*
	 * Page tables are at the end of the trust zone region, but we should
	 * double-check that memlayout and addressmap.c are in sync.
	 *
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
	assert((uintptr_t)_ttb + REGION_SIZE(ttb) == (tz_base_mib + tz_size_mib)
		* MiB && REGION_SIZE(ttb) <= tz_size_mib * MiB);

	mmu_enable();
}
