/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <bootblock_common.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/grf.h>
#include <soc/timer.h>
#include <symbols.h>

void bootblock_soc_init(void)
{
	rkclk_init();

	mmu_init();
	/* Start with a clean slate. */
	mmu_config_range(0, 4096, DCACHE_OFF);
	/* SRAM is tightly wedged between registers, need to use subtables. Map
	 * write-through as equivalent for non-cacheable without XN on A17. */
	mmu_config_range_kb((uintptr_t)_sram/KiB,
			    REGION_SIZE(sram)/KiB, DCACHE_WRITETHROUGH);
	dcache_mmu_enable();

	rkclk_configure_crypto(148500*KHz);
}
