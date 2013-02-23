/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

/* This code is based on src/northbridge/intel/e7520/pciexp_porta.c */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pciexp.h>
#include <arch/io.h>
#include "chip.h"
#include <reset.h>

#define PCIE_LCTL 0x50
#define PCIE_LSTS 0x52

typedef struct northbridge_intel_i3100_config config_t;

static void pcie_init(struct device *dev)
{
}

static unsigned int pcie_scan_bridge(struct device *dev, unsigned int max)
{
	u16 val;
	u16 ctl;
	int flag = 0;
	do {
		val = pci_read_config16(dev, PCIE_LSTS);
		printk(BIOS_DEBUG, "pcie portb link status: %02x\n", val);
		if ((val & (1<<10)) && (!flag)) { /* training error */
			ctl = pci_read_config16(dev, PCIE_LCTL);
			pci_write_config16(dev, PCIE_LCTL, (ctl | (1<<5)));
			val = pci_read_config16(dev, PCIE_LSTS);
			printk(BIOS_DEBUG, "pcie portb reset link status: %02x\n", val);
			flag=1;
			hard_reset();
		}
	} while	(val & (3<<10));
	return pciexp_scan_bridge(dev, max);
}

static struct device_operations pcie_ops  = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = pcie_init,
	.scan_bus         = pcie_scan_bridge,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = 0,
};

static const struct pci_driver pci_driver_0 __pci_driver = {
	.ops    = &pcie_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_3100_PCIE_PB0,
};

static const struct pci_driver pci_driver_1 __pci_driver = {
	.ops    = &pcie_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_3100_PCIE_PB1,
};

static const struct pci_driver pci_driver_2 __pci_driver = {
	.ops    = &pcie_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_3100_PCIE_PB2,
};

static const struct pci_driver pci_driver_3 __pci_driver = {
	.ops    = &pcie_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_3100_PCIE_PB3,
};
