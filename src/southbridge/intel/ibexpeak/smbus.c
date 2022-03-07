/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/path.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <southbridge/intel/common/smbus_ops.h>
#include "pch.h"

static void pch_smbus_init(struct device *dev)
{
	struct resource *res;

	/* Enable clock gating */
	pci_and_config16(dev, 0x80, ~((1 << 8) | (1 << 10) | (1 << 12) | (1 << 14)));

	/* Set Receive Slave Address */
	res = probe_resource(dev, PCI_BASE_ADDRESS_4);
	if (res)
		smbus_set_slave_addr(res->base, SMBUS_SLAVE_ADDR);
}

static struct device_operations smbus_ops = {
	.read_resources		= smbus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_smbus,
	.init			= pch_smbus_init,
	.ops_smbus_bus		= &lops_smbus_bus,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	0x1c22,
	0x1e22,
	PCI_DID_INTEL_IBEXPEAK_SMBUS,
	0
};

static const struct pci_driver pch_smbus __pci_driver = {
	.ops	 = &smbus_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
