/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmu.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>

__weak void soc_mmu_dram_config_post_dram_init(void) { /* no-op */ }

void qc_mmu_dram_config_post_dram_init(void *ddr_base, size_t ddr_size)
{
	mmu_config_range((void *)ddr_base, ddr_size, CACHED_RAM);
	soc_mmu_dram_config_post_dram_init();
}
