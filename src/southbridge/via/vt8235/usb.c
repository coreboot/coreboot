#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>

/* really nothing to do here, both usb 1.1 & 2.0 are normal PCI devices and so get resources allocated
   properly. They are part of the southbridge and are enabled in the chip enable function for the southbridge */

static void usb_init(struct device *dev)
{
	printk(BIOS_DEBUG, "Configuring VIA USB 1.1\n");

	/* pci_write_config8(dev, 0x04, 0x07); */

	/*
	 * To disable; though do we need to do this?
    		pci_write_config8(dev1, 0x3c, 0x00);
    		pci_write_config8(dev1, 0x04, 0x00);

	   Also, on the root dev, for enable:
	   	regval = pci_read_config8(dev0, 0x50);
		regval &= ~(0x36);
		pci_write_config8(dev0, 0x50, regval);

		(regval |= 0x36; for disable)
	 */
}

static struct device_operations usb_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = usb_init,
	.enable           = 0,
	.ops_pci          = 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops    = &usb_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_82C586_2,
};

