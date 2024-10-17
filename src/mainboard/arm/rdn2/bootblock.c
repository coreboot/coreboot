/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmu.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <mainboard/addressmap.h>
#include <symbols.h>

DECLARE_REGION(dev_mem)

void bootblock_mainboard_init(void)
{
	mmu_init();

	/* NOR Flash 0 */
	mmu_config_range((void *)RDN2_FLASH_BASE, (uintptr_t)RDN2_FLASH_SIZE,
			MA_MEM | MA_RO | MA_MEM_NC);

	/* device memory */
	mmu_config_range(_dev_mem, _dram - _dev_mem, MA_DEV | MA_RW);

	/* Set a dummy value for DRAM. ramstage should update the mapping. */
	mmu_config_range(_dram, ((size_t) 2 * GiB) - 16*MiB, MA_MEM | MA_RW);

	mmu_config_range((void *)RDN2_DRAM2_BASE, RDN2_DRAM2_SIZE, MA_MEM | MA_RW);

	mmu_config_range(_ttb, REGION_SIZE(ttb), MA_MEM | MA_S | MA_RW);

	mmu_config_range(_stack, REGION_SIZE(stack), MA_MEM | MA_S | MA_RW);
	mmu_config_range(_cbfs_mcache, REGION_SIZE(cbfs_mcache), MA_MEM | MA_S | MA_RW);
	mmu_config_range(_fmap_cache, REGION_SIZE(fmap_cache), MA_MEM | MA_S | MA_RW);
	mmu_config_range(_timestamp, REGION_SIZE(timestamp), MA_MEM | MA_S | MA_RW);

	mmu_config_range((void *)CONFIG_ECAM_MMCONF_BASE_ADDRESS, CONFIG_ECAM_MMCONF_LENGTH,
			 MA_DEV | MA_RW);
	mmu_enable();
}
