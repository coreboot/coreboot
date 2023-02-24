/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BASECODE_TOM_H
#define SOC_INTEL_COMMON_BASECODE_TOM_H

#include <types.h>

/* Early caching of TOM region */
void early_tom_enable_cache_range(void);

/* Update the TOM if required based on the input TOM address */
void update_tom(uint32_t addr);

#endif
