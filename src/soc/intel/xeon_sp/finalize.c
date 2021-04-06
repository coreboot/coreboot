/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <console/debug.h>
#include <cpu/x86/smm.h>
#include <device/pci.h>
#include <intelpch/lockdown.h>
#include <soc/pci_devs.h>
#include <soc/util.h>

#include "chip.h"

static void lock_pam0123(void)
{
	const struct device *dev;

	if (get_lockdown_config() != CHIPSET_LOCKDOWN_COREBOOT)
		return;

	dev = pcidev_path_on_bus(get_stack_busno(1), PCI_DEVFN(SAD_ALL_DEV, SAD_ALL_FUNC));
	pci_or_config32(dev, SAD_ALL_PAM0123_CSR, PAM_LOCK);
}

static void soc_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	apm_control(APM_CNT_FINALIZE);
	lock_pam0123();

	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, soc_finalize, NULL);
