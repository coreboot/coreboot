/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_MEDIATEK_COMMON_MMU_OPERATIONS_H__
#define __SOC_MEDIATEK_COMMON_MMU_OPERATIONS_H__

#include <arch/mmu.h>
#include <symbols.h>

enum {
	DEV_MEM			= MA_DEV | MA_S  | MA_RW,
	SECURE_CACHED_MEM	= MA_MEM | MA_S  | MA_RW,
	SECURE_UNCACHED_MEM	= MA_MEM | MA_S  | MA_RW | MA_MEM_NC,
	NONSECURE_CACHED_MEM	= MA_MEM | MA_NS | MA_RW,
	NONSECURE_UNCACHED_MEM	= MA_MEM | MA_NS | MA_RW | MA_MEM_NC,
};

DECLARE_REGION(sram_l2c)

void mtk_soc_after_dram(void);
void mtk_soc_disable_l2c_sram(void);

void mtk_mmu_init(void);
void mtk_mmu_after_dram(void);
void mtk_mmu_disable_l2c_sram(void);

#endif
