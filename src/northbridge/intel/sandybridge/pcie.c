/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Patrick Rudolph <siro@das-labor.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <assert.h>

static void pcie_disable(struct device *dev)
{
	printk(BIOS_INFO, "%s: Disabling device\n", dev_path(dev));
	dev->enabled = 0;
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
static const char *pcie_acpi_name(const struct device *dev)
{
	assert(dev);

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	assert(dev->bus);
	if (dev->bus->secondary == 0)
		switch (dev->path.pci.devfn) {
		case PCI_DEVFN(1, 0):
			return "PEGP";
		case PCI_DEVFN(1, 1):
			return "PEG1";
		case PCI_DEVFN(1, 2):
			return "PEG2";
		case PCI_DEVFN(6, 0):
			return "PEG6";
		};

	struct device *const port = dev->bus->dev;
	assert(port);
	assert(port->bus);

	if (dev->path.pci.devfn == PCI_DEVFN(0, 0) &&
	    port->bus->secondary == 0 &&
	    (port->path.pci.devfn == PCI_DEVFN(1, 0) ||
	    port->path.pci.devfn == PCI_DEVFN(1, 1) ||
	    port->path.pci.devfn == PCI_DEVFN(1, 2) ||
	    port->path.pci.devfn == PCI_DEVFN(6, 0)))
		return "DEV0";

	return NULL;
}
#endif

static void
pcie_set_subsystem(struct device *dev, unsigned int ven, unsigned int device)
{
	/* NOTE: This is not the default position! */
	if (!ven || !device)
		pci_write_config32(dev, 0x94,
				   pci_read_config32(dev, 0));
	else
		pci_write_config32(dev, 0x94,
				   ((device & 0xffff) << 16) | (ven & 0xffff));
}

static struct pci_operations pci_ops = {
	.set_subsystem = pcie_set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pciexp_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.disable		= pcie_disable,
	.init			= pci_dev_init,
	.ops_pci		= &pci_ops,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name		= pcie_acpi_name,
#endif
};

static const unsigned short pci_device_ids[] = { 0x0101, 0x0105, 0x0109, 0x010d,
						 0x0151, 0x0155, 0x0159, 0x015d,
						 0 };

static const struct pci_driver pch_pcie __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};
