#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>

/*
 * Enable the ethernet device and turn off stepping (because it is integrated
 * inside the southbridge)
 */
static void nic_init(struct device *dev)
{
	uint8_t		byte;

	printk(BIOS_DEBUG, "Configuring VIA LAN\n");

	/* We don't need stepping - though the device supports it */
	byte = pci_read_config8(dev, PCI_COMMAND);
	byte &= ~PCI_COMMAND_WAIT;
	pci_write_config8(dev, PCI_COMMAND, byte);
}

static struct device_operations nic_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = nic_init,
	.enable           = 0,
	.ops_pci          = 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops	= &nic_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_8233_7,
};
