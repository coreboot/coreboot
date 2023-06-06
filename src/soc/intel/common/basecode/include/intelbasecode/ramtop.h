/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BASECODE_RAMTOP_H
#define SOC_INTEL_COMMON_BASECODE_RAMTOP_H

#include <types.h>

/* Early caching of top_of_ram region */
void early_ramtop_enable_cache_range(void);

/* Update the RAMTOP if required based on the input top_of_ram address */
void update_ramtop(uint32_t addr);

/* Get RAMTOP */
uint32_t get_ramtop_addr(void);

#endif
