/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <soc/thermal.h>

void thermal_cls_sram(void)
{
	int i = 0;
	const uint32_t pattern = 0x27BC86AA;
	uint32_t *buff = (uint32_t *)THERMAL_SRAM_BASE;
	for (i = 0; i < THERMAL_SRAM_LEN / sizeof(*buff); i++) {
		*buff = pattern;
		buff++;
	}
	dcache_clean_invalidate_by_mva((void *)THERMAL_SRAM_BASE, THERMAL_SRAM_LEN);
}

void thermal_stat_cls_sram(void)
{
	int i = 0;
	const uint32_t pattern = 0xFFFFFFFF;
	uint32_t *buff = (uint32_t *)THERMAL_STAT_SRAM_BASE;
	for (i = 0; i < THERMAL_STAT_SRAM_LEN / sizeof(*buff); i++) {
		*buff = pattern;
		buff++;
	}
	dcache_clean_invalidate_by_mva((void *)THERMAL_STAT_SRAM_BASE, THERMAL_STAT_SRAM_LEN);
}

__weak void thermal_write_reboot_temp_sram(uint32_t value)
{
}

__weak void thermal_write_reboot_msr_sram(unsigned int idx, uint32_t value)
{
}
