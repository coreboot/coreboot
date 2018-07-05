/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
#include <device/pci.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/intel/broadwell/chip.h>

static void pci_domain_set_resources(struct device *dev)
{
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources    = &pci_domain_read_resources,
	.set_resources     = &pci_domain_set_resources,
	.scan_bus          = &pci_domain_scan_bus,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.write_acpi_tables = &northbridge_write_acpi_tables,
#endif
};

static struct device_operations cpu_bus_ops = {
	.read_resources   = DEVICE_NOOP,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init             = &broadwell_init_cpus,
};

static void broadwell_enable(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_PCI) {
		/* Handle PCH device enable */
		if (PCI_SLOT(dev->path.pci.devfn) > SA_DEV_SLOT_MINIHD &&
		    (dev->ops == NULL || dev->ops->enable == NULL)) {
			broadwell_pch_enable_dev(dev);
		}
	}
}

struct chip_operations soc_intel_broadwell_ops = {
	CHIP_NAME("Intel Broadwell")
	.enable_dev = &broadwell_enable,
	.init       = &broadwell_init_pre_device,
};

static void pci_set_subsystem(struct device *dev, unsigned int vendor,
	unsigned int device)
{
	if (!vendor || !device)
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   pci_read_config32(dev, PCI_VENDOR_ID));
	else
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   (device << 16) | vendor);
}

struct pci_operations broadwell_pci_ops = {
	.set_subsystem = &pci_set_subsystem
};
