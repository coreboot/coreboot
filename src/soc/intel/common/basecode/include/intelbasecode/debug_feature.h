/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BASECODE_DEBUG_FEATURE_H
#define SOC_INTEL_COMMON_BASECODE_DEBUG_FEATURE_H

#include <types.h>

/*
 * Reads OEM Section area in the Descriptor Region and
 * populates pre_mem_debug structure.
 */
uint8_t pre_mem_debug_init(void);

#endif
