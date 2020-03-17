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

#include <arch/cache.h>
#include <arch/exception.h>
#include <arch/hlt.h>
#include <arch/stages.h>
#include <program_loading.h>
#include <soc/cache.h>
#include <soc/early_configs.h>
#include <symbols.h>
#include <vendorcode/google/chromeos/chromeos.h>

static void enable_cache(void)
{
	mmu_init();
	/* Whole space is uncached. */
	mmu_config_range(0, 4096, DCACHE_OFF);
	/* SRAM is cached. MMU code will round size up to page size. */
	mmu_config_range((uintptr_t)_sram/MiB,
			 DIV_ROUND_UP(REGION_SIZE(sram), MiB),
			 DCACHE_WRITEBACK);
	mmu_disable_range(0, 1);
	dcache_mmu_enable();
}

void verstage_mainboard_init(void)
{
	/* Do the minimum to run vboot at full speed */
	configure_l2_cache();
	enable_cache();
	early_mainboard_init();
}

void stage_entry(uintptr_t unused)
{
	asm volatile ("bl arm_init_caches"
		      : : : "r0", "r1", "r2", "r3", "r4", "r5", "ip");
	main();
}
