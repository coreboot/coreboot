/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmu.h>
#include <bootblock_common.h>
#include <symbols.h>

DECLARE_REGION(flash);
void bootblock_mainboard_init(void)
{
	mmu_init();

	/* Everything below DRAM is device memory */
	mmu_config_range((void *)0, (uintptr_t)_dram, MA_DEV | MA_RW);
	/* Set a dummy value for DRAM. ramstage should update the mapping. */
	mmu_config_range(_dram, ((size_t) CONFIG_DRAM_SIZE_MB) * MiB, MA_MEM | MA_RW);

	mmu_config_range(_flash, REGION_SIZE(flash), MA_MEM | MA_RO | MA_MEM_NC);

	mmu_config_range(_ttb, REGION_SIZE(ttb), MA_MEM | MA_S | MA_RW);
	mmu_config_range(_bootblock, REGION_SIZE(bootblock), MA_MEM | MA_S | MA_RW);
	mmu_config_range(_ramstage, REGION_SIZE(ramstage), MA_MEM | MA_S | MA_RW);
	mmu_config_range((void *)CONFIG_ECAM_MMCONF_BASE_ADDRESS, CONFIG_ECAM_MMCONF_LENGTH,
			 MA_DEV | MA_RW);
	mmu_enable();
}
