/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/path.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <southbridge/intel/common/smbus_ops.h>
#include <soc/smbus.h>

static void pch_smbus_init(struct device *dev)
{
	struct resource *res;
	u16 reg16;

	/* Enable clock gating */
	/* FIXME: Using 32-bit ops with a 16-bit variable is a bug! These should be 16-bit! */
	reg16 = pci_read_config32(dev, 0x80);
	reg16 &= ~((1 << 8)|(1 << 10)|(1 << 12)|(1 << 14));
	pci_write_config32(dev, 0x80, reg16);

	/* Set Receive Slave Address */
	res = find_resource(dev, PCI_BASE_ADDRESS_4);
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
	0x9c22, /* LynxPoint */
	0x9ca2, /* WildcatPoint */
	0
};

static const struct pci_driver pch_smbus __pci_driver = {
	.ops	 = &smbus_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
