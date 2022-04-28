/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BASECODE_DEBUG_FEATURE_H
#define SOC_INTEL_COMMON_BASECODE_DEBUG_FEATURE_H

#include <types.h>

/* Get cpu and pch tracehub modes defined in the OEM Section of descriptor region */
void debug_get_pch_cpu_tracehub_modes(uint8_t *cpu_tracehub_mode, uint8_t *pch_trachub_mode);

/* Check if CSE firmware update is enabled or not */
bool is_debug_cse_fw_update_disable(void);

/*
 * Reads OEM Section area in the Descriptor Region and
 * populates debug_feature_cntrl structure.
 */
enum cb_err dbg_feature_cntrl_init(void);

#endif
