/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <intelblocks/rmt_plus.h>
#include <security/vboot/vboot_common.h>

void enable_rmt_plus(FSP_M_CONFIG *mem_cfg)
{
#if CONFIG(SOC_INTEL_RMT_PLUS)
	enable_rmt_plus_platform(mem_cfg);
#elif CONFIG(SOC_INTEL_RMT_PLUS_IN_DEV_MODE)
	if (vboot_developer_mode_enabled())
		enable_rmt_plus_platform(mem_cfg);
#endif
}
