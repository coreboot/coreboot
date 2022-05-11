/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/path.h>
#include <device/smbus.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/smbus.h>
#include <device/smbus_host.h>
#include <intelblocks/tco.h>
#include "smbuslib.h"

static int lsmbus_read_byte(struct device *dev, u8 address)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;
	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, PCI_BASE_ADDRESS_4);
	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(struct device *dev, u8 address, u8 data)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, PCI_BASE_ADDRESS_4);
	return do_smbus_write_byte(res->base, device, address, data);
}

static int lsmbus_read_block(struct device *dev, u8 cmd, u8 bytes, u8 *buffer)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;
	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, PCI_BASE_ADDRESS_4);
	return do_smbus_block_read(res->base, device, cmd, bytes, buffer);
}

static int lsmbus_write_block(struct device *dev, u8 cmd, u8 bytes, const u8 *buffer)
{
	u16 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);
	res = find_resource(pbus->dev, PCI_BASE_ADDRESS_4);
	return do_smbus_block_write(res->base, device, cmd, bytes, (u8 *)buffer);
}

static struct smbus_bus_operations lops_smbus_bus = {
	.read_byte	= lsmbus_read_byte,
	.write_byte	= lsmbus_write_byte,
	.block_read	= lsmbus_read_block,
	.block_write	= lsmbus_write_block,
};

static void pch_smbus_init(struct device *dev)
{
	struct resource *res;

	/* Enable clock gating */
	pci_update_config32(dev, 0x80,
		~((1 << 8) | (1 << 10) | (1 << 12) | (1 << 14)), 0);

	/* Set Receive Slave Address */
	res = probe_resource(dev, PCI_BASE_ADDRESS_4);
	if (res)
		smbus_set_slave_addr(res->base, SMBUS_SLAVE_ADDR);
}

static void smbus_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);

	struct resource *res = new_resource(dev, PCI_BASE_ADDRESS_4);
	res->base = SMBUS_IO_BASE;
	res->size = 32;
	res->limit = res->base + res->size - 1;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_RESERVE |
		     IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}

/*
 * `finalize_smbus` function is native implementation of equivalent events
 * performed by each FSP NotifyPhase() API invocations.
 *
 * Operations are:
 * 1. TCO Lock.
 */
static void finalize_smbus(struct device *dev)
{
	if (!CONFIG(USE_FSP_NOTIFY_PHASE_POST_PCI_ENUM))
		tco_lockdown();
}

static struct device_operations smbus_ops = {
	.read_resources		= smbus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_smbus,
	.init			= pch_smbus_init,
	.ops_pci		= &pci_dev_ops_pci,
	.ops_smbus_bus		= &lops_smbus_bus,
	.final			= finalize_smbus,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_MTL_SMBUS,
	PCI_DID_INTEL_APL_SMBUS,
	PCI_DID_INTEL_CNL_SMBUS,
	PCI_DID_INTEL_CNP_H_SMBUS,
	PCI_DID_INTEL_SPT_LP_SMBUS,
	PCI_DID_INTEL_SPT_H_SMBUS,
	PCI_DID_INTEL_LWB_SMBUS_SUPER,
	PCI_DID_INTEL_LWB_SMBUS,
	PCI_DID_INTEL_ICP_LP_SMBUS,
	PCI_DID_INTEL_CMP_SMBUS,
	PCI_DID_INTEL_CMP_H_SMBUS,
	PCI_DID_INTEL_TGP_LP_SMBUS,
	PCI_DID_INTEL_TGP_H_SMBUS,
	PCI_DID_INTEL_MCC_SMBUS,
	PCI_DID_INTEL_JSP_SMBUS,
	PCI_DID_INTEL_ADP_P_SMBUS,
	PCI_DID_INTEL_ADP_S_SMBUS,
	PCI_DID_INTEL_ADP_M_N_SMBUS,
	PCI_DID_INTEL_DNV_SMBUS_LEGACY,
	0
};

static const struct pci_driver pch_smbus __pci_driver = {
	.ops	 = &smbus_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices	 = pci_device_ids,
};
