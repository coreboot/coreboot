/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#include <lib.h>
#include <string.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <chip.h>

static void pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
static const char *domain_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";
	return NULL;
}
#endif

static struct device_operations pci_domain_ops = {
	.read_resources   = pci_domain_read_resources,
	.set_resources    = pci_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = pci_domain_scan_bus,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name        = domain_acpi_name
#endif
};

static struct device_operations cpu_bus_ops = {
	.read_resources   = DEVICE_NOOP,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init             = broadwell_de_init_cpus,
	.scan_bus         = NULL,
};

static void enable_dev(device_t dev)
{
	printk(BIOS_DEBUG, "enable_dev(%s, %d)\n",
	       dev_name(dev), dev->path.type);

	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_PCI) {
		/* Handle south cluster enablement. */
		if (PCI_SLOT(dev->path.pci.devfn) > 0 &&
		    (dev->ops == NULL || dev->ops->enable == NULL)) {
			southcluster_enable_dev(dev);
		}
	}
}

/* Called at BS_DEV_INIT_CHIPS time -- very early. Just after BS_PRE_DEVICE. */
static void soc_init(void *chip_info)
{
	broadwell_de_init_pre_device();
}

struct chip_operations soc_intel_fsp_broadwell_de_ops = {
	CHIP_NAME("Intel(R) Xeon(R) Processor D-1500 Product Family")
	.enable_dev = enable_dev,
	.init = soc_init,
};

static void pci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

struct pci_operations soc_pci_ops = {
	.set_subsystem = &pci_set_subsystem,
};
