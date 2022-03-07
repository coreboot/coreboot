/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "i82801gx.h"

static void pci_init(struct device *dev)
{
	u16 reg16;

	/* Enable Bus Master */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* This device has no interrupt */
	pci_write_config8(dev, INTR, 0xff);

	/* Disable parity error response and SERR */
	pci_and_config16(dev, PCI_BRIDGE_CONTROL,
			 ~(PCI_BRIDGE_CTL_PARITY | PCI_BRIDGE_CTL_SERR));

	/* Master Latency Count must be set to 0x04! */
	pci_update_config8(dev, SMLT, 0x07, 0x04 << 3);

	/* Clear errors in status registers. FIXME: Do something? */
	reg16 = pci_read_config16(dev, PSTS);
	//reg16 |= 0xf900;
	pci_write_config16(dev, PSTS, reg16);

	reg16 = pci_read_config16(dev, SECSTS);
	// reg16 |= 0xf900;
	pci_write_config16(dev, SECSTS, reg16);
}

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= pci_init,
	.scan_bus		= pci_scan_bridge,
	.ops_pci		= &pci_dev_ops_pci,
};

/* Desktop */
/* 82801BA/CA/DB/EB/ER/FB/FR/FW/FRW/GB/GR/GDH/HB/IB/6300ESB/i3100 */
static const struct pci_driver i82801g_pci __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= 0x244e,
};

/* Mobile / Ultra Mobile */
/* 82801BAM/CAM/DBL/DBM/FBM/GBM/GHM/GU/HBM/HEM */
static const struct pci_driver i82801gmu_pci __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= 0x2448,
};
