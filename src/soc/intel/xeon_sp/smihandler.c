/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/smihandler.h>
#include <soc/pm.h>
#include <cpu/x86/smm.h>

/* This is needed by common SMM code */
const smi_handler_t southbridge_smi[SMI_STS_BITS] = {
	[APM_STS_BIT] = smihandler_southbridge_apmc,
	[PM1_STS_BIT] = smihandler_southbridge_pm1,
#if CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_TCO_ENABLE)
	[TCO_STS_BIT] = smihandler_southbridge_tco,
#endif
};
