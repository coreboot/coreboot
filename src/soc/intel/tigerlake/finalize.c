/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Tiger Lake Processor PCH Datasheet
 * Document number: 575857
 * Chapter number: 4, 29
 */

#include <arch/io.h>
#include <device/mmio.h>
#include <bootstate.h>
#include <console/console.h>
#include <console/post_codes.h>
#include <cpu/x86/smm.h>
#include <device/pci.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/tco.h>
#include <intelblocks/thermal.h>
#include <spi-generic.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <soc/smbus.h>
#include <soc/soc_chip.h>
#include <soc/systemagent.h>

static void pch_finalize(void)
{
	config_t *config;

	/* TCO Lock down */
	tco_lockdown();

	/* TODO: Add Thermal Configuration */

	/*
	 * Disable ACPI PM timer based on dt policy
	 *
	 * Disabling ACPI PM timer is necessary for XTAL OSC shutdown.
	 * Disabling ACPI PM timer also switches off TCO
	 *
	 * SA_DEV_ROOT device is used here instead of PCH_DEV_PMC since it is
	 * just required to get to chip config. PCH_DEV_PMC is hidden by this
	 * point and hence removed from the root bus. pcidev_path_on_root thus
	 * returns NULL for PCH_DEV_PMC device.
	 */
	config = config_of_soc();
	if (config->PmTimerDisabled)
		pmc_disable_acpi_timer();

	pmc_clear_pmcon_sts();
}

static void tbt_finalize(void)
{
	int i;
	const struct device *dev;

	/* Disable Thunderbolt PCIe root ports bus master */
	for (i = 0; i < NUM_TBT_FUNCTIONS; i++) {
		dev = pcidev_path_on_root(SA_DEVFN_TBT(i));
		if (dev)
			pci_dev_disable_bus_master(dev);
	}
}

static void soc_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	pch_finalize();
	apm_control(APM_CNT_FINALIZE);
	tbt_finalize();

	/* Indicate finalize step with post code */
	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, soc_finalize, NULL);
