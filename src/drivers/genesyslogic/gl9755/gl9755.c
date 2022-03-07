/* SPDX-License-Identifier: GPL-2.0-only */

/* Driver for Genesys Logic GL9755 */

#include <console/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "gl9755.h"

static void gl9755_enable(struct device *dev)
{
	uint32_t reg;

	printk(BIOS_INFO, "GL9755: configure ASPM and LTR\n");

	/* Set Vendor Config to be configurable */
	pci_or_config32(dev, CFG, CFG_EN);

	/* Set LTR value */
	pci_write_config32(dev, LTR, NO_SNOOP_SCALE|NO_SNOOP_VALUE|SNOOP_SCALE|SNOOP_VALUE);

	/* Adjust L1 exit latency to enable ASPM */
	reg = pci_read_config32(dev, CFG2);
	reg &= ~CFG2_LAT_L1_MASK;
	reg |= CFG2_LAT_L1_64US;
	pci_write_config32(dev, CFG2, reg);

	/* Disable ASPM L0s support */
	pci_and_config32(dev, CFG2, ~CFG2_L0S_SUPPORT);

	/* Turn off debug mode to enable SCP/OCP */
	pci_and_config32(dev, CFG3, ~SCP_DEBUG);

	/* Set Vendor Config to be non-configurable */
	pci_and_config32(dev, CFG, ~CFG_EN);
}

static struct device_operations gl9755_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.enable			= gl9755_enable
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_GLI_9755,
	0
};

static const struct pci_driver genesyslogic_gl9755 __pci_driver = {
	.ops		= &gl9755_ops,
	.vendor		= PCI_VID_GLI,
	.devices	= pci_device_ids,
};

struct chip_operations drivers_generic_genesyslogic_gl9755_ops = {
	CHIP_NAME("Genesys Logic GL9755")
};
