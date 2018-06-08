/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <device/cardbus.h>
#include "pci7420.h"
#include "chip.h"

static void pci7420_firewire_init(struct device *dev)
{
	printk(BIOS_DEBUG, "TI PCI7420/7620 FireWire init\n");

#ifdef ODD_IRQ_FIXUP
	/* This is a workaround for buggy kernels. This should
	 * probably be read from the device tree, but as long
	 * as only one mainboard is using this bridge it does
	 * not matter
	 */
	pci_write_config8(dev, PCI_INTERRUPT_PIN, INTC);
#endif
}

static struct device_operations ti_pci7420_firewire_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = pci7420_firewire_init,
};

static const struct pci_driver ti_pci7420_driver __pci_driver = {
	.ops    = &ti_pci7420_firewire_ops,
	.vendor = 0x104c,
	.device = 0x802e,
};

static void ti_pci7420_firewire_enable_dev(struct device *dev)
{
	/* Nothing here yet */
}

struct chip_operations southbridge_ti_pci7420_firewire_ops = {
	CHIP_NAME("Texas Instruments PCI7420/7620 FireWire (IEEE 1394)")
	.enable_dev    = ti_pci7420_firewire_enable_dev,
};
