/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <reg_script.h>

#include <soc/device_nvs.h>
#include <soc/iosf.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include "chip.h"

static const struct reg_script emmc_ops[] = {
	REG_SCRIPT_END,
};

static void emmc_init(struct device *dev)
{
	struct soc_intel_braswell_config *config = config_of(dev);

	printk(BIOS_DEBUG, "eMMC init\n");
	reg_script_run_on_dev(dev, emmc_ops);

	if (config->emmc_acpi_mode) {
		printk(BIOS_DEBUG, "Enabling ACPI mode\n");
		scc_enable_acpi_mode(dev, SCC_MMC_CTL, SCC_NVS_MMC);
	}
}

static struct device_operations emmc_device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= emmc_init,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver southcluster __pci_driver = {
	.ops		= &emmc_device_ops,
	.vendor		= PCI_VID_INTEL,
	.device		= MMC_DEVID,
};
