/*
 * (C) 2003 Linux Networx
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static void pcix_init(device_t dev)
{
	return;
}

static struct device_operations pcix_ops  = {
        .read_resources = pci_bus_read_resources,
        .set_resources = pci_dev_set_resources,
        .init = pcix_init,
        .scan_bus = pci_scan_bridge,
};

static struct pci_driver pcix_driver __pci_driver = {
        .ops    = &pcix_ops,
        .vendor = PCI_VENDOR_ID_AMD,
        .device = 0x7450,
};


static void ioapic_enable(device_t dev)
{
	uint32_t value;
	value = pci_read_config32(dev, 0x44);
	if (dev->enable) {
		value |= ((1 << 1) | (1 << 0));
	} else {
		value &= ~((1 << 1) | (1 << 0));
	}
	pci_write_config32(dev, 0x44, value);
}

static struct device_operations ioapic_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources  = pci_dev_set_resources,
	.init     = 0,
	.scan_bus = 0,
	.enable   = ioapic_enable,
};

static struct pci_driver ioapic_driver __pci_driver = {
	.ops    = &ioapic_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x7451,
	
};
