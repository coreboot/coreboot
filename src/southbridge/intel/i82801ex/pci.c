#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801ex.h"

static void pci_init(struct device *dev)
{
	uint16_t word;

	/* Clear system errors */
	word = pci_read_config16(dev, 0x06);
	word |= 0xf900; /* Clear possible errors */
	pci_write_config16(dev, 0x06, word);

#if 0
	/* System error enable */
	uint32_t dword;
	dword = pci_read_config32(dev, 0x04);
	dword |= (1<<8); /* SERR# Enable */
	dword |= (1<<6); /* Parity Error Response */
	pci_write_config32(dev, 0x04, dword);
#endif

	word = pci_read_config16(dev, 0x1e);
	word |= 0xf800; /* Clear possible errors */
	pci_write_config16(dev, 0x1e, word);
}

static struct device_operations pci_ops  = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = pci_init,
	.scan_bus         = pci_scan_bridge,
	.ops_pci          = 0,
};

static const struct pci_driver pci_driver __pci_driver = {
	.ops    = &pci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801ER_PCI,
};

