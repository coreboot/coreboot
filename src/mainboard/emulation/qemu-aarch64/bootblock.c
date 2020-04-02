/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/mmu.h>
#include <bootblock_common.h>
#include <symbols.h>

extern u8 _secram[], _esecram[];

void bootblock_mainboard_init(void)
{
	mmu_init();

	/* Everything below DRAM is device memory */
	mmu_config_range((void *)0, (uintptr_t)_dram, MA_DEV | MA_RW);
	/* Set a dummy value for DRAM. ramstage should update the mapping. */
	mmu_config_range(_dram, 1 * GiB, MA_MEM | MA_RW);

	mmu_config_range(_ttb, REGION_SIZE(ttb), MA_MEM | MA_S | MA_RW);
	mmu_config_range(_bootblock, REGION_SIZE(bootblock), MA_MEM | MA_S | MA_RW);
	mmu_config_range(_romstage, REGION_SIZE(romstage), MA_MEM | MA_S | MA_RW);
	mmu_config_range(_ramstage, REGION_SIZE(ramstage), MA_MEM | MA_S | MA_RW);

	mmu_config_range(_secram, REGION_SIZE(secram), MA_MEM | MA_S | MA_RW);

	mmu_enable();
}
