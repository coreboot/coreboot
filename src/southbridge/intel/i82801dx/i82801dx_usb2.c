//2003 Copywright Tyan

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801dx.h"

static void usb2_init(struct device *dev)
{


#if 0  
  uint32_t cmd;
	printk_debug("USB: Setting up controller.. ");
	cmd = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, 
		cmd | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | 
		PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE);


	printk_debug("done.\n");
#endif
}

static struct device_operations usb2_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = usb2_init,
	.scan_bus         = 0,
	.enable           = i82801dx_enable,
};

static const struct pci_driver usb2_driver __pci_driver = {
	.ops    = &usb2_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801DBM_EHCI,
};
