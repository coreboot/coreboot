#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "esb6300.h"

static void uhci_init(struct device *dev)
{
	uint32_t cmd;

#if 1
	printk(BIOS_DEBUG, "UHCI: Setting up controller.. ");
	cmd = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND,
		cmd | PCI_COMMAND_MASTER);


	printk(BIOS_DEBUG, "done.\n");
#endif

}

static struct pci_operations lops_pci = {
	/* The subsystem id follows the ide controller */
	.set_subsystem = 0,
};

static struct device_operations uhci_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = uhci_init,
	.scan_bus         = 0,
	.enable           = esb6300_enable,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver usb1_driver __pci_driver = {
	.ops    = &uhci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_6300ESB_USB1,
};

static const struct pci_driver usb2_driver __pci_driver = {
	.ops    = &uhci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_6300ESB_USB2,
};

/* Note: May or may not need different init than UHCI. */
static const struct pci_driver ehci_driver __pci_driver = {
	.ops    = &uhci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_6300ESB_EHCI,
};

