/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/mmio.h>
#include <bootstate.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <delay.h>
#include <device/pci.h>
#include <intelblocks/cse.h>
#include <intelblocks/gpio.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/tco.h>
#include <intelblocks/thermal.h>
#include <spi-generic.h>
#include <intelpch/lockdown.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <soc/smbus.h>
#include <soc/soc_chip.h>
#include <soc/systemagent.h>
#include <timer.h>

static void pch_handle_sideband(config_t *config)
{

}

static void pch_finalize(void)
{
	config_t *config = config_of_soc();

	/* TCO Lock down */
	tco_lockdown();

	/* TODO: Add Thermal Configuration */

	pch_handle_sideband(config);

	pmc_clear_pmcon_sts();
}

static void tbt_finalize(void)
{
	int i;
	const struct device *dev;

	/* Disable Thunderbolt PCIe root ports bus master */
	for (i = 0; i < NUM_TBT_FUNCTIONS; i++) {
		dev = pcidev_path_on_root(PCI_DEVFN_TBT(i));
		if (dev)
			pci_dev_disable_bus_master(dev);
	}
}

static void sa_finalize(void)
{
	if (get_lockdown_config() == CHIPSET_LOCKDOWN_COREBOOT)
		sa_lock_pam();
}

static void soc_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	pch_finalize();
	apm_control(APM_CNT_FINALIZE);
	tbt_finalize();
	sa_finalize();
	heci_set_to_d0i3();
	if (CONFIG(DISABLE_HECI1_AT_PRE_BOOT))
		heci1_disable();

	/* Indicate finalize step with post code */
	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
/*
 * The purpose of this change is to accommodate more time to push out sending
 * CSE EOP messages at post.
 */
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, soc_finalize, NULL);
