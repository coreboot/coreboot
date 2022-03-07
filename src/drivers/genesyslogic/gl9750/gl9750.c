/* SPDX-License-Identifier: GPL-2.0-only */

/* Driver for Genesys Logic GL9750 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "gl9750.h"

static void gl9750_enable(struct device *dev)
{
	printk(BIOS_INFO, "GL9750: configure ASPM\n");

	/* Set Vendor Config to be configurable */
	pci_or_config32(dev, CFG, CFG_EN);

	/*
	 * When both ASPM L0s and L1 are supported, GL9750 may not enter L1.
	 * So disable L0s support.
	 */
	pci_and_config32(dev, CFG2, ~CFG2_L0S_SUPPORT);

	/* Set Vendor Config to be non-configurable */
	pci_and_config32(dev, CFG, ~CFG_EN);
}

static struct device_operations gl9750_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.enable			= gl9750_enable
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_GLI_9750,
	0
};

static const struct pci_driver genesyslogic_gl9750 __pci_driver = {
	.ops		= &gl9750_ops,
	.vendor		= PCI_VID_GLI,
	.devices	= pci_device_ids,
};

struct chip_operations drivers_generic_genesyslogic_gl9750_ops = {
	CHIP_NAME("Genesys Logic GL9750")
};
