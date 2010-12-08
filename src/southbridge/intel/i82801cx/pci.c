#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801cx.h"

static void pci_init(struct device *dev)
{
	// NOTE: the original (v1) 'CA code set these in the bridge register (0x3E-3F)
	/* Enable pci error detecting */
	uint32_t dword = pci_read_config32(dev, PCI_COMMAND);
	dword |= (PCI_COMMAND_SERR | PCI_COMMAND_PARITY);
	pci_write_config32(dev, PCI_COMMAND, dword);
}

static struct device_operations pci_ops  = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = pci_init,
	.scan_bus         = pci_scan_bridge,
};

static const struct pci_driver pci_driver __pci_driver = {
	.ops    = &pci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801CA_PCI,
};

