/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Alder Lake Processor PCH Datasheet
 * Document number: 621483
 * Chapter number: 4, 29
 */

#include <arch/io.h>
#include <bootstate.h>
#include <commonlib/console/post_codes.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <intelblocks/cse.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/tco.h>
#include <intelblocks/thermal.h>
#include <intelpch/lockdown.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <soc/smbus.h>
#include <soc/soc_chip.h>
#include <soc/systemagent.h>
#include <spi-generic.h>

#define CAMERA1_CLK		0x8000 /* Camera 1 Clock */
#define CAMERA2_CLK		0x8080 /* Camera 2 Clock */
#define CAM_CLK_EN		(1 << 1)
#define MIPI_CLK		(1 << 0)
#define HDPLL_CLK		(0 << 0)

static void pch_enable_isclk(void)
{
	pcr_or32(PID_ISCLK, CAMERA1_CLK, CAM_CLK_EN | MIPI_CLK);
	pcr_or32(PID_ISCLK, CAMERA2_CLK, CAM_CLK_EN | MIPI_CLK);
}

static void pch_handle_sideband(config_t *config)
{
	if (config->pch_isclk)
		pch_enable_isclk();
}

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
	 */
	config = config_of_soc();
	if (config->PmTimerDisabled)
		pmc_disable_acpi_timer();

	pch_handle_sideband(config);

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

static void sa_finalize(void)
{
	if (get_lockdown_config() == CHIPSET_LOCKDOWN_COREBOOT)
		sa_lock_pam();
}

static void heci_finalize(void)
{
	unsigned int cse_dev[] = {
		PCH_DEVFN_CSE,
		PCH_DEVFN_CSE_2,
		PCH_DEVFN_CSE_3,
		PCH_DEVFN_CSE_4
	};

	for (int i = 0; i < ARRAY_SIZE(cse_dev); i++) {
		if (!is_cse_devfn_visible(cse_dev[i]))
			continue;

		set_cse_device_state(cse_dev[i], DEV_IDLE);
	}
}

static void soc_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	pch_finalize();
	apm_control(APM_CNT_FINALIZE);
	tbt_finalize();
	sa_finalize();
	heci_finalize();

	/* Indicate finalize step with post code */
	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
/*
 * The purpose of this change is to accommodate more time to push out sending
 * CSE EOP messages at post.
 */
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, soc_finalize, NULL);
