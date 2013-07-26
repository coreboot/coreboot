/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <soc/gpio.h>
#include <soc/lpc.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/cpu.h>
#include <delay.h>

static void pch_pcie_init(struct device *dev)
{
	u16 reg16;
	u32 reg32;

	printk(BIOS_DEBUG, "Initializing PCH PCIe bridge.\n");

	/* Enable SERR */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_SERR;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Enable Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Set Cache Line Size to 0x10 */
	pci_write_config8(dev, 0x0c, 0x10);

	reg16 = pci_read_config16(dev, 0x3e);
	reg16 &= ~(1 << 0); /* disable parity error response */
	reg16 |= (1 << 2); /* ISA enable */
	pci_write_config16(dev, 0x3e, reg16);

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
	pci_write_config16(dev, 0x06, reg16);
	reg16 = pci_read_config16(dev, 0x1e);
	pci_write_config16(dev, 0x1e, reg16);
}

static void pcie_set_L1_ss_max_latency(device_t dev, unsigned int off)
{
	/* Set max snoop and non-snoop latency for the SOC */
	pci_write_config32(dev, off, 0x10031003);
}

static struct pci_operations pcie_ops = {
	.set_L1_ss_latency = pcie_set_L1_ss_max_latency,
};

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= pch_pcie_init,
	.enable			= NULL,
	.scan_bus		= pciexp_scan_bridge,
	.ops_pci		= &pcie_ops,
};

static const unsigned short pcie_device_ids[] = {
	/* Sunrisepoint-LP */
	0x9d10, 0x9d11, 0x9d12, 0x9d13, 0x9d14, 0x9d15, 0x9d16, 0x9d17,
	0x9d18, 0x9d19, 0x9d1a, 0x9d1b,
	0
};

static const struct pci_driver pch_pcie __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pcie_device_ids,
};
