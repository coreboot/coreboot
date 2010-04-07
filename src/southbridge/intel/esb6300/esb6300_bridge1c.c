#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "esb6300.h"

static void bridge1c_init(struct device *dev)
{
	/* configuration */
	pci_write_config8(dev, 0x1b, 0x30);
//	pci_write_config8(dev, 0x3e, 0x07);
	pci_write_config8(dev, 0x3e, 0x04);  /* parity ignore */
	pci_write_config8(dev, 0x6c, 0x0c);  /* undocumented  */
	pci_write_config8(dev, 0xe0, 0x20);

	/* SRB enable */
	pci_write_config16(dev, 0xe4, 0x0232);

	/* Burst size */
	pci_write_config8(dev, 0xf0, 0x02);

	/* prefetch threshold size */
	pci_write_config16(dev, 0xf8, 0x2121);

	/* primary latency */
	pci_write_config8(dev, 0x0d, 0x28);

	/* multi transaction timer */
	pci_write_config8(dev, 0x42, 0x08);
}

static struct device_operations pci_ops  = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = bridge1c_init,
	.scan_bus         = pci_scan_bridge,
	.ops_pci          = 0,
};

static const struct pci_driver pci_driver __pci_driver = {
	.ops    = &pci_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_6300ESB_PCI_X,
};

