/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <southbridge/intel/common/smbus_ops.h>
#include "i82801hx.h"

static void ich8_smbus_init(struct device *dev)
{
	/* Enable clock gating */
	pci_and_config16(dev, 0x80, ~((1 << 8) | (1 << 10) | (1 << 12) | (1 << 14)));
}

static struct device_operations smbus_ops = {
	.read_resources		= smbus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_smbus,
	.init			= ich8_smbus_init,
	.ops_smbus_bus		= &lops_smbus_bus,
	.ops_pci		= &pci_dev_ops_pci,
};

static const struct pci_driver ich8_smbus __pci_driver = {
	.ops	 	= &smbus_ops,
	.vendor		= PCI_VID_INTEL,
	.device		= PCI_DID_INTEL_82801HB_SMB,
};
