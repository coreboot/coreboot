/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>

#include "vx900.h"

/**
 * @file vx900/pcie.c
 *
 * STATUS:
 * We do part of the sequence to initialize the PCIE link. The problem is that
 * the reset signal for each slot is connected to a GPO pin, but We don't know
 * which GPO pin. We need to figure out which GPIO pin is hooked to which slot,
 * and have a mechanism to specify this per-mainboard (devicetree.cb).
 *
 * There is currently no timeout detection mechanism for when a link comes up.
 * If the link never comes up, we hang.
 */

static void vx900_pcie_link_init(struct device *dev)
{
	u8 reg8;
	u32 reg32;

	u8 fn = dev->path.pci.devfn & 0x07;

	/* Step 1 : Check for presence of PCIE device */
	reg8 = pci_read_config8(dev, 0x5a);

	if (reg8 & (1 << 6))
		printk(BIOS_DEBUG, "Card detected in PEX%i\n", fn);
	else
		return;

	/* Step 2: Wait for device to enter L0 state */
	/* FIXME: implement timeout detection */
	while (0x8a != pci_read_config8(dev, 0x1c3));

	/* Step 3: Clear PCIe error status, then check for failures */
	pci_write_config32(dev, 0x104, 0xffffffff);
	reg32 = pci_read_config32(dev, 0x104);
	if (0 != reg32) {
		printk(BIOS_DEBUG, "PEX init error. flags 0x%.8x\n", reg32);
		return;
	}

	pci_write_config32(dev, 0x110, 0xffffffff);
	reg32 = pci_read_config32(dev, 0x110);
	if (0 != reg32)
		printk(BIOS_DEBUG, "PEX errors. flags 0x%.8x\n", reg32);

	pci_write_config8(dev, 0xa4, 0xff);
	if (pci_read_config8(dev, 0x4a) & (1 << 3))
		printk(BIOS_DEBUG, "Unsupported request detected.\n");

	pci_write_config8(dev, 0x15a, 0xff);
	if (pci_read_config8(dev, 0x15a) & (1 << 1))
		printk(BIOS_DEBUG, "Negotiation pending.\n");

	/* Step 4: Read vendor ID */
	/* FIXME: Do we want to run through the whole sequence and delay boot
	 * by several seconds if the device does not respond properly the first
	 * time? */
}

static void vx900_pex_dev_set_resources(struct device *dev)
{
	assign_resources(dev->link_list);
}

static void vx900_pex_init(struct device *dev)
{
	/* FIXME: For some reason, PEX0 hangs on init. Find issue, fix it. */
	if ((dev->path.pci.devfn & 0x7) == 0)
		return;

	vx900_pcie_link_init(dev);
}

static struct device_operations vx900_pex_ops = {
	.read_resources = pci_bus_read_resources,
	.set_resources = vx900_pex_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init = vx900_pex_init,
	.scan_bus = pciexp_scan_bridge,
	.reset_bus = pci_bus_reset,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_VIA_VX900_PEX1,
	PCI_DEVICE_ID_VIA_VX900_PEX2,
	PCI_DEVICE_ID_VIA_VX900_PEX3,
	PCI_DEVICE_ID_VIA_VX900_PEX4,
	0,
};

static const struct pci_driver pex_driver __pci_driver = {
	.ops = &vx900_pex_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.devices = pci_device_ids,

};
