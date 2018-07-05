/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <reg_script.h>

#include <soc/iosf.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include "chip.h"

static const struct reg_script emmc_ops[] = {
	REG_SCRIPT_END,
};

static void emmc_init(struct device *dev)
{
	struct soc_intel_braswell_config *config = dev->chip_info;

	printk(BIOS_SPEW, "%s/%s ( %s )\n",
			__FILE__, __func__, dev_name(dev));
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
	.vendor		= PCI_VENDOR_ID_INTEL,
	.device		= MMC_DEVID,
};
