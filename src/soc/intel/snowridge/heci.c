/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/resource.h>

static void heci_read_resources(struct device *dev)
{
	struct resource *res = NULL;

	pci_dev_read_resources(dev);

	/**
	 * Clear `IORESOURCE_PCI64` since we want this device to remain under 4G as it is used
	 * by FSP Notify.
	 */
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	res->limit = 0xffffffff;
	res->flags &= ~IORESOURCE_PCI64;
}

static struct device_operations snr_heci_ops = {
	.read_resources = heci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.ops_pci = &pci_dev_ops_pci,
};

static const struct pci_driver snr_heci_driver __pci_driver = {
	.ops = &snr_heci_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_SNR_HECI1,
};
