/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp_util.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

static void pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources   = &pci_domain_read_resources,
	.set_resources    = &pci_domain_set_resources,
	.scan_bus         = &pci_domain_scan_bus,
	.ops_pci_bus      = &pci_bus_default_ops,
};

static struct device_operations cpu_bus_ops = {
	.init             = &soc_init_cpus,
};

static void soc_enable(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_PCI) {
		/* Handle PCH device enable */
		if (PCI_SLOT(dev->path.pci.devfn) > SA_DEV_SLOT_IGD &&
		    (dev->ops == NULL || dev->ops->enable == NULL)) {
			pch_enable_dev(dev);
		}
	}
}

struct chip_operations soc_intel_skylake_ops = {
	CHIP_NAME("Intel Skylake")
	.enable_dev = &soc_enable,
	.init       = &soc_init_pre_device,
};

/* UPD parameters to be initialized before SiliconInit */
void soc_silicon_init_params(SILICON_INIT_UPD *params)
{
	const struct device *dev;
	const struct soc_intel_skylake_config *config;

	/* Set the parameters for SiliconInit */
	dev = dev_find_slot(0, PCI_DEVFN(PCH_DEV_SLOT_LPC, 0));
	if (!dev || !dev->chip_info)
		return;
	config = dev->chip_info;

	params->Device4Enable = config->Device4Enable;
}

void soc_display_silicon_init_params(const SILICON_INIT_UPD *original,
	SILICON_INIT_UPD *params)
{
	/* Display the parameters for SiliconInit */
	printk(BIOS_SPEW, "UPD values for SiliconInit:\n");

	soc_display_upd_value("GpioTablePtr", 4,
		(uint32_t)original->GpioTablePtr,
		(uint32_t)params->GpioTablePtr);
	soc_display_upd_value("Device4Enable", 1,
		original->Device4Enable,
		params->Device4Enable);
}

static void pci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device)
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   pci_read_config32(dev, PCI_VENDOR_ID));
	else
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   (device << 16) | vendor);
}

struct pci_operations soc_pci_ops = {
	.set_subsystem = &pci_set_subsystem
};
