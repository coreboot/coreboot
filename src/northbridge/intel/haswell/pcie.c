/*
 * This file is part of the coreboot project.
 *
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

#if CONFIG(HAVE_ACPI_TABLES)
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
		};

	struct device *const port = dev->bus->dev;
	assert(port);
	assert(port->bus);

	if (dev->path.pci.devfn == PCI_DEVFN(0, 0) &&
	    port->bus->secondary == 0 &&
	    (port->path.pci.devfn == PCI_DEVFN(1, 0) ||
	    port->path.pci.devfn == PCI_DEVFN(1, 1) ||
	    port->path.pci.devfn == PCI_DEVFN(1, 2)))
		return "DEV0";

	return NULL;
}
#endif

static struct pci_operations pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
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
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= pcie_acpi_name,
#endif
};

static const unsigned short pci_device_ids[] = { 0x0c01, 0x0c05, 0x0c09, 0x0c0d, 0 };

static const struct pci_driver pch_pcie __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};
