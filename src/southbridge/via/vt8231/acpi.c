#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>

static void acpi_init(struct device *dev)
{
	printk(BIOS_DEBUG, "Configuring VIA ACPI\n");

	// Set ACPI base address to IO 0x4000
	pci_write_config32(dev, 0x48, 0x4001);

	// Enable ACPI access (and setup like award)
	pci_write_config8(dev, 0x41, 0x84);

	// Set hardware monitor base address to IO 0x6000
	pci_write_config32(dev, 0x70, 0x6001);

	// Enable hardware monitor (and setup like award)
	pci_write_config8(dev, 0x74, 0x01);

	// set IO base address to 0x5000
	pci_write_config32(dev, 0x90, 0x5001);

	// Enable SMBus
	pci_write_config8(dev, 0xd2, 0x01);
}

static struct device_operations acpi_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = acpi_init,
	.enable           = 0,
	.ops_pci          = 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops	= &acpi_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_8231_4,
};
