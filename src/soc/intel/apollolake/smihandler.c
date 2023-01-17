/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/smm.h>
#include <cpu/intel/em64t100_save_state.h>
#include <gpio.h>
#include <intelblocks/smihandler.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

int smihandler_soc_disable_busmaster(pci_devfn_t dev)
{
	if (dev == PCH_DEV_PMC)
		return 0;
	return 1;
}

const struct smm_save_state_ops *get_smm_save_state_ops(void)
{
	return &em64t100_smm_ops;
}

const smi_handler_t southbridge_smi[32] = {
	[SMI_ON_SLP_EN_STS_BIT] = smihandler_southbridge_sleep,
	[APM_STS_BIT] = smihandler_southbridge_apmc,
	[PM1_STS_BIT] = smihandler_southbridge_pm1,
	[GPIO_STS_BIT] = smihandler_southbridge_gpi,
#if CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_TCO_ENABLE)
	[TCO_STS_BIT] = smihandler_southbridge_tco,
#endif
	[PERIODIC_STS_BIT] = smihandler_southbridge_periodic,
#if CONFIG(SOC_ESPI)
	[ESPI_SMI_STS_BIT] = smihandler_southbridge_espi,
#endif
};
