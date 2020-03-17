/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_QUALCOMM_MMU_COMMON_H_
#define _SOC_QUALCOMM_MMU_COMMON_H_

#include <commonlib/region.h>
#include <soc/symbols_common.h>

#define   CACHED_RAM (MA_MEM | MA_S | MA_RW)
#define UNCACHED_RAM (MA_MEM | MA_S | MA_RW | MA_MEM_NC)
#define      DEV_MEM (MA_DEV | MA_S | MA_RW)

static struct region * const ddr_region = (struct region *)_ddr_information;

void soc_mmu_dram_config_post_dram_init(void);
void qc_mmu_dram_config_post_dram_init(void *ddr_base, size_t ddr_size);

#endif  /* _SOC_QUALCOMM_MMU_COMMON_H_ */
