/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_MEDIATEK_COMMON_MMU_OPERATIONS_H__
#define __SOC_MEDIATEK_COMMON_MMU_OPERATIONS_H__

#include <arch/mmu.h>

enum {
	DEV_MEM			= MA_DEV | MA_S  | MA_RW,
	SECURE_CACHED_MEM	= MA_MEM | MA_S  | MA_RW,
	SECURE_UNCACHED_MEM	= MA_MEM | MA_S  | MA_RW | MA_MEM_NC,
	NONSECURE_CACHED_MEM	= MA_MEM | MA_NS | MA_RW,
	NONSECURE_UNCACHED_MEM	= MA_MEM | MA_NS | MA_RW | MA_MEM_NC,
};

extern unsigned char _sram_l2c[];
extern unsigned char _esram_l2c[];
#define _sram_l2c_size (_esram_l2c - _sram_l2c)

void mtk_soc_after_dram(void);
void mtk_soc_disable_l2c_sram(void);

void mtk_mmu_init(void);
void mtk_mmu_after_dram(void);
void mtk_mmu_disable_l2c_sram(void);

#endif
