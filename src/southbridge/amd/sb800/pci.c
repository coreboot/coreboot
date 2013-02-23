/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "sb800.h"

static void pci_init(struct device *dev)
{
	u32 dword;
	u8 byte;

	/* RPR 6.1 Enables the PCI-bridge subtractive decode */
	/* This setting is strongly recommended since it supports some legacy PCI add-on cards,such as BIOS debug cards */
	byte = pci_read_config8(dev, 0x4B);
	byte |= 1 << 7;
	pci_write_config8(dev, 0x4B, byte);
	byte = pci_read_config8(dev, 0x40);
	byte |= 1 << 5;
	pci_write_config8(dev, 0x40, byte);

	/* RPR6.2 PCI-bridge upstream dual address window */
	/* this setting is applicable if the system memory is more than 4GB,and the PCI device can support dual address access */
	byte = pci_read_config8(dev, 0x50);
	byte |= 1 << 0;
	pci_write_config8(dev, 0x50, byte);

	/* RPR 6.3 Enable One-Prefetch-Channel Mode */
	dword = pci_read_config32(dev, 0x64);
	dword |= 1 << 20;
	pci_write_config32(dev, 0x64, dword);

	/* rpr6.4 Adjusting CLKRUN# */
	dword = pci_read_config32(dev, 0x64);
	dword |= (1 << 15);
	pci_write_config32(dev, 0x64, dword);
}

static struct pci_operations lops_pci = {
	.set_subsystem = 0,
};

static struct device_operations pci_ops = {
	.read_resources = pci_bus_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init = pci_init,
	.scan_bus = pci_scan_bridge,
	.reset_bus = pci_bus_reset,
	.ops_pci = &lops_pci,
};

static const struct pci_driver pci_driver __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_PCI,
};
