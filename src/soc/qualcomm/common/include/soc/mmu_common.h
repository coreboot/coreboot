/* SPDX-License-Identifier: GPL-2.0-only */

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
bool soc_modem_carve_out(void **start, void **end);

#endif  /* _SOC_QUALCOMM_MMU_COMMON_H_ */
