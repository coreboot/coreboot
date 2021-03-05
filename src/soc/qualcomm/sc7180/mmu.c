/* SPDX-License-Identifier: GPL-2.0-only */

#include <symbols.h>
#include <arch/mmu.h>
#include <arch/cache.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>

void sc7180_mmu_init(void)
{
	mmu_init();

	mmu_config_range((void *)(4 * KiB), ((4UL * GiB) - (4 * KiB)), DEV_MEM);
	mmu_config_range((void *)_ssram, REGION_SIZE(ssram), CACHED_RAM);
	mmu_config_range((void *)_bsram, REGION_SIZE(bsram), CACHED_RAM);
	mmu_config_range((void *)_dma_coherent, REGION_SIZE(dma_coherent),
			 UNCACHED_RAM);

	mmu_enable();
}

void soc_mmu_dram_config_post_dram_init(void)
{
	mmu_config_range((void *)_aop_code_ram, REGION_SIZE(aop_code_ram), CACHED_RAM);
	mmu_config_range((void *)_aop_data_ram, REGION_SIZE(aop_data_ram), CACHED_RAM);
}
