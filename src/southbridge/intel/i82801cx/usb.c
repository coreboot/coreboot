#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801cx.h"

static void usb_init(struct device *dev)
{

#if 0
	uint32_t cmd;
	printk(BIOS_DEBUG, "USB: Setting up controller.. ");
	cmd = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND,
		cmd | PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
		PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE);


	printk(BIOS_DEBUG, "done.\n");
#endif

}

static struct device_operations usb_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = usb_init,
	.scan_bus         = 0,
	.enable           = i82801cx_enable,
};

static const struct pci_driver usb_driver_1 __pci_driver = {
	.ops    = &usb_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801CA_USB1,
};
static const struct pci_driver usb_driver_2 __pci_driver = {
        .ops    = &usb_ops,
        .vendor = PCI_VENDOR_ID_INTEL,
        .device = PCI_DEVICE_ID_INTEL_82801CA_USB2,
};
static const struct pci_driver usb_driver_3 __pci_driver = {
        .ops    = &usb_ops,
        .vendor = PCI_VENDOR_ID_INTEL,
        .device = PCI_DEVICE_ID_INTEL_82801CA_USB3,
};

