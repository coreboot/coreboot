/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_RMT_PLUS_H
#define SOC_INTEL_COMMON_BLOCK_RMT_PLUS_H

#include <fsp/api.h>

/*
 * Platform-specific implementation. Each SoC that supports RMT+
 * (Rank Margin Test Plus) must implement this to configure the FSPM
 * UPDs required to trigger per-bit margin testing and BDAT publication.
 */
void enable_rmt_plus_platform(FSP_M_CONFIG *mem_cfg);

/*
 * Common wrapper. Calls enable_rmt_plus_platform() conditionally based
 * on SOC_INTEL_RMT_PLUS / SOC_INTEL_RMT_PLUS_IN_DEV_MODE Kconfig and,
 * for the dev-mode variant, vboot developer-mode state.
 */
void enable_rmt_plus(FSP_M_CONFIG *mem_cfg);

#endif /* SOC_INTEL_COMMON_BLOCK_RMT_PLUS_H */
