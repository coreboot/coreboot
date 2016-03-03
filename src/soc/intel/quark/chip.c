/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2016 Intel Corporation.
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

#include <console/console.h>
#include <device/device.h>
#include <soc/ramstage.h>

static void chip_init(void *chip_info)
{
	/* Perform silicon specific init. */
	if (IS_ENABLED(CONFIG_RELOCATE_FSP_INTO_DRAM))
		intel_silicon_init();
	else
		fsp_run_silicon_init(find_fsp(CONFIG_FSP_ESRAM_LOC), 0);
}

static void pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources	= pci_domain_read_resources,
	.set_resources	= pci_domain_set_resources,
	.scan_bus	= pci_domain_scan_bus,
	.ops_pci_bus	= pci_bus_default_ops,
};

static void chip_enable_dev(device_t dev)
{
	const char *type_name = dev_path_name(dev->path.type);

	/* Set the operations if it is a special bus type */
	printk(BIOS_DEBUG, "type: %s\n", type_name);
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
}

struct chip_operations soc_intel_quark_ops = {
	CHIP_NAME("Intel Quark")
	.init		= &chip_init,
	.enable_dev	= chip_enable_dev,
};

void soc_silicon_init_params(SILICON_INIT_UPD *params)
{
	struct soc_intel_quark_config *config;
	device_t dev;

	/* Locate the configuration data from devicetree.cb */
	dev = dev_find_slot(0, LPC_DEV_FUNC);
	if (!dev) {
		printk(BIOS_ERR,
			"Error! Device (PCI:0:%02x.%01x) not found, "
			"soc_silicon_init_params!\n", PCI_DEVICE_NUMBER_QNC_LPC,
			PCI_FUNCTION_NUMBER_QNC_LPC);
		return;
	}
	config = dev->chip_info;

	/* Set the parameters for SiliconInit */
//	printk(BIOS_DEBUG, "Updating UPD values for SiliconInit\n");
}

void soc_display_silicon_init_params(const SILICON_INIT_UPD *old,
	SILICON_INIT_UPD *new)
{
	/* Display the parameters for SiliconInit */
//	printk(BIOS_SPEW, "UPD values for SiliconInit:\n");
}
