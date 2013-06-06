#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <usbdebug.h>
#include "i82801ex.h"

static void ehci_init(struct device *dev)
{
	uint32_t cmd;

	printk(BIOS_DEBUG, "EHCI: Setting up controller.. ");
	cmd = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND,
		cmd | PCI_COMMAND_MASTER);

	printk(BIOS_DEBUG, "done.\n");
}

static void ehci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	uint8_t access_cntl;
	access_cntl = pci_read_config8(dev, 0x80);
	/* Enable writes to protected registers */
	pci_write_config8(dev, 0x80, access_cntl | 1);
	/* Write the subsystem vendor and device id */
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
		((device & 0xffff) << 16) | (vendor & 0xffff));
	/* Restore protection */
	pci_write_config8(dev, 0x80, access_cntl);
}

static struct pci_operations lops_pci = {
	.set_subsystem = &ehci_set_subsystem,
};
static struct device_operations ehci_ops  = {
	.read_resources   = pci_ehci_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ehci_init,
	.scan_bus         = 0,
	.enable           = i82801ex_enable,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver ehci_driver __pci_driver = {
	.ops    = &ehci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801ER_EHCI,
};
