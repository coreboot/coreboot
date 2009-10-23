/*
 * Initialisation of the PCI bridge .
 */

#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>

static void 
pci_bridge_enable(struct device *dev)
{
	printk_info("Configure PCI Bridge\n");

	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_MEMORY|PCI_COMMAND_MASTER);
	pci_write_config16(dev, 0x60, 0x0f00);

	printk_info("PCI Bridge configuration complete\n");
}

struct device_operations pci_bridge_ops  = {
        .read_resources   = pci_dev_read_resources,
        .set_resources    = pci_dev_set_resources,
        .enable_resources = pci_dev_enable_resources,
        .enable           = pci_bridge_enable,
        .scan_bus         = 0,
};

struct pci_driver pci_bridge_pci_driver __pci_driver = {
	.ops = &pci_bridge_ops,
	.device = PCI_DEVICE_ID_IBM_405GP,
	.vendor = PCI_VENDOR_ID_IBM,
};
