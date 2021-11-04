/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <arch/exception.h>
#include <arch/hlt.h>
#include <arch/stages.h>
#include <program_loading.h>
#include <security/vboot/vboot_common.h>
#include <soc/cache.h>
#include <soc/early_configs.h>
#include <symbols.h>

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
