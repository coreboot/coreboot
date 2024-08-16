/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/symbols.h>
#include <soc/mmu_operations.h>

/* mtk_soc_after_dram is called in romstage */
void mtk_soc_after_dram(void)
{
	mmu_config_range(_dram_dma, REGION_SIZE(dram_dma),
			 NONSECURE_UNCACHED_MEM);
}
