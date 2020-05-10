/* SPDX-License-Identifier: GPL-2.0-only */

#include <symbols.h>
#include <arch/mmu.h>
#include <arch/cache.h>
#include <soc/mmu.h>
#include <soc/symbols.h>

void qcs405_mmu_init(void)
{
	mmu_init();

	mmu_config_range((void *)(4 * KiB), ((4UL * GiB) - (4 * KiB)),
			MA_DEV | MA_S | MA_RW);
	mmu_config_range((void *)_ssram, REGION_SIZE(ssram), MA_MEM | MA_S | MA_RW);
	mmu_config_range((void *)_bsram, REGION_SIZE(bsram), MA_MEM | MA_S | MA_RW);

	mmu_enable();
}
