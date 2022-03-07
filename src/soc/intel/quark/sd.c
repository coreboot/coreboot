/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/sdhci.h>
#include <commonlib/storage.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/storage_test.h>

static void init(struct device *dev)
{
	/* Run the SD test */
	if (CONFIG(STORAGE_TEST)) {
		uint32_t bar;
		uint32_t previous_bar;
		uint16_t previous_command;

		bar = storage_test_init(dev, &previous_bar, &previous_command);
		storage_test(bar, 0);
		storage_test_complete(dev, previous_bar, previous_command);
	}
}

static const struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= init,
};

static const struct pci_driver pmc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= 0x08A7,
};
