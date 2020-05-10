/* SPDX-License-Identifier: GPL-2.0-only */

#include <symbols.h>
#include <soc/addressmap.h>
#include <soc/mmu.h>
#include <soc/sdram.h>
#include <arch/mmu.h>

void soc_mmu_init(void)
{
	const unsigned long devmem = MA_DEV | MA_S | MA_RW;
	const unsigned long secure_mem = MA_MEM | MA_S | MA_RW;

	mmu_init();

	/*
	 * Need to use secure mem attribute, as firmware is running in ARM TZ
	 * region.
	 */
	mmu_config_range((void *)_ttb, REGION_SIZE(ttb), secure_mem);
	mmu_config_range((void *)_dram, sdram_size_mb() * MiB, secure_mem);
	/* IO space has the MSB set and is divided into 4 sub-regions:
	 * * NCB
	 * * SLI
	 * * RSL
	 * * AP
	 */
	mmu_config_range((void *)IO_SPACE_START, IO_SPACE_SIZE, devmem);

	mmu_enable();
}
