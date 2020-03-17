/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "i82801gx.h"

static void pci_init(struct device *dev)
{
	u16 reg16;
	u8 reg8;

	/* Enable Bus Master */
	reg16 = pci_read_config16(dev, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MASTER;
	pci_write_config16(dev, PCI_COMMAND, reg16);

	/* This device has no interrupt */
	pci_write_config8(dev, INTR, 0xff);

	/* disable parity error response and SERR */
	reg16 = pci_read_config16(dev, PCI_BRIDGE_CONTROL);
	reg16 &= ~PCI_BRIDGE_CTL_PARITY;
	reg16 &= ~PCI_BRIDGE_CTL_SERR;
	pci_write_config16(dev, PCI_BRIDGE_CONTROL, reg16);

	/* Master Latency Count must be set to 0x04! */
	reg8 = pci_read_config8(dev, SMLT);
	reg8 &= 0x07;
	reg8 |= (0x04 << 3);
	pci_write_config8(dev, SMLT, reg8);

	/* Clear errors in status registers */
	reg16 = pci_read_config16(dev, PSTS);
	//reg16 |= 0xf900;
	pci_write_config16(dev, PSTS, reg16);

	reg16 = pci_read_config16(dev, SECSTS);
	// reg16 |= 0xf900;
	pci_write_config16(dev, SECSTS, reg16);
}

static struct pci_operations pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= pci_init,
	.scan_bus		= pci_scan_bridge,
	.ops_pci		= &pci_ops,
};

/* Desktop */
/* 82801BA/CA/DB/EB/ER/FB/FR/FW/FRW/GB/GR/GDH/HB/IB/6300ESB/i3100 */
static const struct pci_driver i82801g_pci __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x244e,
};

/* Mobile / Ultra Mobile */
/* 82801BAM/CAM/DBL/DBM/FBM/GBM/GHM/GU/HBM/HEM */
static const struct pci_driver i82801gmu_pci __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x2448,
};
