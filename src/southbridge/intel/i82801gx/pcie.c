/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static void pci_init(struct device *dev)
{
	u16 reg16;
	u32 reg32;

	printk(BIOS_DEBUG, "Initializing ICH7 PCIe bridge.\n");

	/* Enable Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Set Cache Line Size to 0x10 */
	// This has no effect but the OS might expect it
	pci_write_config8(dev, 0x0c, 0x10);

	reg16 = pci_read_config16(dev, 0x3e);
	reg16 &= ~(1 << 0); /* disable parity error response */
	// reg16 &= ~(1 << 1); /* disable SERR */
	reg16 |= (1 << 2); /* ISA enable */
	pci_write_config16(dev, 0x3e, reg16);

	/* Enable IO xAPIC on this PCIe port */
	reg32 = pci_read_config32(dev, 0xd8);
	reg32 |= (1 << 7);
	pci_write_config32(dev, 0xd8, reg32);

	/* Enable Backbone Clock Gating */
	reg32 = pci_read_config32(dev, 0xe1);
	reg32 |= (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0);
	pci_write_config32(dev, 0xe1, reg32);

	/* Set VC0 transaction class */
	reg32 = pci_read_config32(dev, 0x114);
	reg32 &= 0xffffff00;
	reg32 |= 1;
	pci_write_config32(dev, 0x114, reg32);

	/* Mask completion timeouts */
	reg32 = pci_read_config32(dev, 0x148);
	reg32 |= (1 << 14);
	pci_write_config32(dev, 0x148, reg32);

	/* Enable common clock configuration */
	// Are there cases when we don't want that?
	reg16 = pci_read_config16(dev, 0x50);
	reg16 |= (1 << 6);
	pci_write_config16(dev, 0x50, reg16);

#ifdef EVEN_MORE_DEBUG
	reg32 = pci_read_config32(dev, 0x20);
	printk(BIOS_SPEW, "    MBL    = 0x%08x\n", reg32);
	reg32 = pci_read_config32(dev, 0x24);
	printk(BIOS_SPEW, "    PMBL   = 0x%08x\n", reg32);
	reg32 = pci_read_config32(dev, 0x28);
	printk(BIOS_SPEW, "    PMBU32 = 0x%08x\n", reg32);
	reg32 = pci_read_config32(dev, 0x2c);
	printk(BIOS_SPEW, "    PMLU32 = 0x%08x\n", reg32);
#endif

	/* Clear errors in status registers */
	reg16 = pci_read_config16(dev, 0x06);
	//reg16 |= 0xf900;
	pci_write_config16(dev, 0x06, reg16);

	reg16 = pci_read_config16(dev, 0x1e);
	//reg16 |= 0xf900;
	pci_write_config16(dev, 0x1e, reg16);
}

static void pcie_set_subsystem(struct device *dev, unsigned int vendor,
			       unsigned int device)
{
	/* NOTE: This is not the default position! */
	if (!vendor || !device) {
		pci_write_config32(dev, 0x94,
				pci_read_config32(dev, 0));
	} else {
		pci_write_config32(dev, 0x94,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations pci_ops = {
	.set_subsystem = pcie_set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= pci_init,
	.scan_bus		= pci_scan_bridge,
	.ops_pci		= &pci_ops,
};

static const unsigned short i82801gx_pcie_ids[] = {
	0x27d0, /* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
	0x27d2, /* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
	0x27d4, /* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
	0x27d6, /* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
	0x27e0, /* 82801GR/GDH/GHM (ICH7R/ICH7DH/ICH7-M DH) */
	0x27e2, /* 82801GR/GDH/GHM (ICH7R/ICH7DH/ICH7-M DH) */
	0
};

static const struct pci_driver i82801gx_pcie __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= i82801gx_pcie_ids,
};
