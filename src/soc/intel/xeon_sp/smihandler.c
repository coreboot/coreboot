/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/pci.h>
#include <intelblocks/smihandler.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

/*
 * Specific SOC SMI handler during ramstage finalize phase
 */
void smihandler_soc_at_finalize(void)
{
	/* SMM_FEATURE_CONTROL can only be written within SMM. */
	printk(BIOS_DEBUG, "Lock SMM_FEATURE_CONTROL\n");
	const pci_devfn_t dev = UBOX_DEV_PMON;
	pci_or_config32(dev, SMM_FEATURE_CONTROL,
			SMM_CODE_CHK_EN | SMM_FEATURE_CONTROL_LOCK);

}

/* This is needed by common SMM code */
const smi_handler_t southbridge_smi[SMI_STS_BITS] = {
	[APM_STS_BIT] = smihandler_southbridge_apmc,
	[PM1_STS_BIT] = smihandler_southbridge_pm1,
#if CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_TCO_ENABLE)
	[TCO_STS_BIT] = smihandler_southbridge_tco,
#endif
};
