/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <arch/cache.h>
#include <soc/thermal_internal.h>
#include <string.h>

/* SRAM for Thermal */
#define THERMAL_SRAM_BASE		(_mcufw_reserved + 0x1000)
#define THERMAL_SRAM_LEN		0x400

static void thermal_cls_sram(void)
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

/* SRAM for Thermal state */
#define THERMAL_STAT_SRAM_BASE		((uintptr_t)_sram + 0x0001A800)
#define THERMAL_STAT_SRAM_LEN		0x400

static void thermal_stat_cls_sram(void)
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
