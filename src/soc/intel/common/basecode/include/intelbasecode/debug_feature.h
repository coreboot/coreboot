/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BASECODE_DEBUG_FEATURE_H
#define SOC_INTEL_COMMON_BASECODE_DEBUG_FEATURE_H

#include <types.h>

/* Check if CSE firmware update is enabled or not */
bool is_debug_cse_fw_update_disable(void);

/*
 * Reads OEM Section area in the Descriptor Region and
 * populates pre_mem_debug structure.
 */
enum cb_err pre_mem_debug_init(void);

#endif
