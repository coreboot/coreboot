/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/pmc.h>
#include <soc/iomap.h>
#include <soc/pm.h>
#include <soc/pmc.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

static void pch_power_options(struct device *dev) { /* TODO */ }

/* Fill up PMC resource structure */
int pmc_soc_get_resources(struct pmc_resource_config *cfg)
{
	cfg->pwrmbase_offset = PMC_PWRM_BASE;
	cfg->pwrmbase_addr = DEFAULT_PWRM_BASE;
	cfg->pwrmbase_size = DEFAULT_PWRM_SIZE;
	cfg->abase_offset = PMC_ACPI_BASE;
	cfg->abase_addr = DEFAULT_PMBASE;
	cfg->abase_size = DEFAULT_PMBASE_SIZE;

	return 0;
}

static void pch_set_acpi_mode(void)
{
	if (!acpi_is_wakeup_s3()) {
		apm_control(APM_CNT_ACPI_DISABLE);
	}
}

void pmc_soc_init(struct device *dev)
{
	uint32_t pwrm_base = pci_read_config32(dev, PMC_PWRM_BASE) & MASK_PMC_PWRM_BASE;

	printk(BIOS_DEBUG, "pch: %s\n", __func__);

	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER |
						     PCI_COMMAND_MEMORY |
						     PCI_COMMAND_IO);

	/* Setup power options. */
	pch_power_options(dev);

	/* Configure ACPI mode. */
	pch_set_acpi_mode();

	/*
	 * Disable ACPI PM timer based on Kconfig
	 *
	 * Disabling ACPI PM timer is necessary for XTAL OSC shutdown.
	 * Disabling ACPI PM timer also switches off TCO.
	 */
	if (!CONFIG(USE_PM_ACPI_TIMER))
		setbits8((volatile void *)(uintptr_t)(pwrm_base + PCH_PWRM_ACPI_TMR_CTL),
			 ACPI_TIM_DIS);
}
