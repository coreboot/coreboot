/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <arch/cache.h>
#include <assert.h>
#include <device/mmio.h>
#include <soc/thermal.h>

/* SRAM for Thermal */
#define THERMAL_REBOOT_TEMP_SRAM_OFFSET	0x39C
#define THERMAL_REBOOT_MSR_SRAM_OFFSET	0x340
#define THERMAL_REBOOT_MSR_SRAM_LEN	(6 * 4)

/* SRAM for GPU Thermal state */
#define GPU_THERMAL_STAT_SRAM_BASE	((uintptr_t)_sram + 0x00017C00)
#define GPU_THERMAL_STAT_SRAM_LEN	0x400

static void thermal_gpu_stat_cls_sram(void)
{
	int i = 0;
	const uint32_t pattern = 0xFFFFFFFF;
	uint32_t *buff = (uint32_t *)GPU_THERMAL_STAT_SRAM_BASE;
	for (i = 0; i < GPU_THERMAL_STAT_SRAM_LEN / sizeof(*buff); i++) {
		*buff = pattern;
		buff++;
	}
	dcache_clean_invalidate_by_mva((void *)GPU_THERMAL_STAT_SRAM_BASE,
				       GPU_THERMAL_STAT_SRAM_LEN);
}

void thermal_sram_init(void)
{
	thermal_cls_sram();
	thermal_stat_cls_sram();
	thermal_gpu_stat_cls_sram();
}

void thermal_write_reboot_temp_sram(uint32_t value)
{
	write32(THERMAL_SRAM_BASE + THERMAL_REBOOT_TEMP_SRAM_OFFSET, value);
}

void thermal_write_reboot_msr_sram(unsigned int idx, uint32_t value)
{
	unsigned int offset = 0;

	assert((idx * 4) < THERMAL_REBOOT_MSR_SRAM_LEN);
	offset = THERMAL_REBOOT_MSR_SRAM_OFFSET + idx * 4;
	write32(THERMAL_SRAM_BASE + offset, value);
}
