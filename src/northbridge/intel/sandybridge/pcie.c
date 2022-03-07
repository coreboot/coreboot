/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <assert.h>

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

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pciexp_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.init			= pci_dev_init,
	.ops_pci		= &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= pcie_acpi_name,
#endif
};

static const unsigned short pci_device_ids[] = {
	0x0101, 0x0105, 0x0109, 0x010d,
	0x0151, 0x0155, 0x0159, 0x015d,
	0,
};

static const struct pci_driver pch_pcie __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};
