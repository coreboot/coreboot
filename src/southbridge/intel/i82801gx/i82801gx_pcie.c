/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static void pci_init(struct device *dev)
{
	u16 reg16;
	u32 reg32;

	printk_debug("Initializing ICH7 PCIe bridge.\n");
#if 0
	// When the latency of the PCIe(!) bridge is set to 0x20
	// all devices on the secondary bus of the PCI(!) bridge
	// suddenly vanish. If you know why, please explain here.

	/* Set latency timer to 32. */
	pci_write_config16(dev, 0x1b, 0x20);
#endif

	/* disable parity error response */
	reg16 = pci_read_config16(dev, 0x3e);
	reg16 &= ~(1 << 0);
	pci_write_config16(dev, 0x3e, reg16);

	/* Clear errors in status registers */
	reg16 = pci_read_config16(dev, 0x06);
	reg16 |= 0xf900;
	pci_write_config16(dev, 0x06, reg16);

	reg16 = pci_read_config16(dev, 0x1e);
	reg16 |= 0xf900;
	pci_write_config16(dev, 0x1e, reg16);

	reg32 = pci_read_config32(dev, 0x20);
	printk_debug("    MBL    = 0x%08x\n", reg32);
	reg32 = pci_read_config32(dev, 0x24);
	printk_debug("    PMBL   = 0x%08x\n", reg32);
	reg32 = pci_read_config32(dev, 0x28);
	printk_debug("    PMBU32 = 0x%08x\n", reg32);
	reg32 = pci_read_config32(dev, 0x2c);
	printk_debug("    PMLU32 = 0x%08x\n", reg32);
}

static void set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	u32 pci_id;

	printk_debug("Setting PCIe bridge subsystem ID.\n");
	pci_id = pci_read_config32(dev, 0);
	pci_write_config32(dev, 0x94, pci_id );
}

static struct pci_operations pci_ops = {
	.set_subsystem = set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= pci_init,
	.scan_bus		= pci_scan_bridge,
	.ops_pci		= &pci_ops,
};

/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
static const struct pci_driver i82801gx_pcie_port1 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27d0,
};

/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
static const struct pci_driver i82801gx_pcie_port2 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27d2,
};

/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
static const struct pci_driver i82801gx_pcie_port3 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27d4,
};

/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
static const struct pci_driver i82801gx_pcie_port4 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27d6,
};

/* 82801GR/GDH/GHM (ICH7R/ICH7DH/ICH7-M DH) */
static const struct pci_driver i82801gx_pcie_port5 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27e0,
};

/* 82801GR/GDH/GHM (ICH7R/ICH7DH/ICH7-M DH) */
static const struct pci_driver i82801gx_pcie_port6 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27e2,
};
