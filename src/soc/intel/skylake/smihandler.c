/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <intelblocks/smihandler.h>
#include <soc/pm.h>

const smi_handler_t southbridge_smi[SMI_STS_BITS] = {
	[SMI_ON_SLP_EN_STS_BIT] = smihandler_southbridge_sleep,
	[APM_STS_BIT] = smihandler_southbridge_apmc,
	[PM1_STS_BIT] = smihandler_southbridge_pm1,
	[GPE0_STS_BIT] = smihandler_southbridge_gpe0,
	[GPIO_STS_BIT] = smihandler_southbridge_gpi,
	[ESPI_SMI_STS_BIT] = smihandler_southbridge_espi,
	[MCSMI_STS_BIT] = smihandler_southbridge_mc,
#if CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_TCO_ENABLE)
	[TCO_STS_BIT] = smihandler_southbridge_tco,
#endif
	[PERIODIC_STS_BIT] = smihandler_southbridge_periodic,
	[MONITOR_STS_BIT] = smihandler_southbridge_monitor,
};
