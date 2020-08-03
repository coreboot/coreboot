/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>

#include <arch/cache.h>
#include <bootblock_common.h>
#include <symbols.h>

#define SRAM_START          ((uintptr_t)_sram / MiB)
#define SRAM_END            (DIV_ROUND_UP((uintptr_t)_esram, MiB))

#define DRAM_START          ((uintptr_t)_dram / MiB)
#define DRAM_SIZE           (CONFIG_DRAM_SIZE_MB)

void bootblock_soc_init(void)
{
	mmu_init();

	/* Map everything strongly ordered by default */
	mmu_config_range(0, 4096, DCACHE_OFF);

	mmu_config_range(SRAM_START, SRAM_END - SRAM_START,
		DCACHE_WRITEBACK);

	mmu_config_range(DRAM_START, DRAM_SIZE, DCACHE_WRITEBACK);

	dcache_mmu_enable();
}
