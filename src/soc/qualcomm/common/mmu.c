/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmu.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>

__weak bool soc_modem_carve_out(void **start, void **end) { return false; }

void qc_mmu_dram_config_post_dram_init(void *ddr_base, size_t ddr_size)
{
	void *start = NULL;
	void *end = NULL;

	if (!soc_modem_carve_out(&start, &end)) {
		mmu_config_range((void *)ddr_base, ddr_size, CACHED_RAM);
	} else {
		mmu_config_range(ddr_base, start - ddr_base, CACHED_RAM);
		mmu_config_range(end, ddr_base + ddr_size - end, CACHED_RAM);
	}

	mmu_config_range((void *)_aop_code_ram, REGION_SIZE(aop_code_ram),
							CACHED_RAM);
	mmu_config_range((void *)_aop_data_ram, REGION_SIZE(aop_data_ram),
							CACHED_RAM);
}
