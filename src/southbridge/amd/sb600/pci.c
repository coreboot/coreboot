/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
#include "sb600.h"

static void pci_init(struct device *dev)
{
	u32 dword;
	u16 word;
	u8 byte;

	/* RPR 4.1 Enables the PCI-bridge subtractive decode */
	/* This setting is strongly recommended since it supports some legacy PCI add-on cards,such as BIOS debug cards */
	byte = pci_read_config8(dev, 0x4B);
	byte |= 1 << 7;
	pci_write_config8(dev, 0x4B, byte);
	byte = pci_read_config8(dev, 0x40);
	byte |= 1 << 5;
	pci_write_config8(dev, 0x40, byte);

	/* RPR4.2 PCI-bridge upstream dual address window */
	/* this setting is applicable if the system memory is more than 4GB,and the PCI device can support dual address access */
	byte = pci_read_config8(dev, 0x50);
	byte |= 1 << 0;
	pci_write_config8(dev, 0x50, byte);

	/* RPR 4.3 PCI bus 64-byte DMA read access */
	/* Enhance the PCI bus DMA performance */
	byte = pci_read_config8(dev, 0x4B);
	byte |= 1 << 4;
	pci_write_config8(dev, 0x4B, byte);

	/* RPR 4.4 Enables the PCIB writes to be cacheline aligned. */
	/* The size of the writes will be set in the Cacheline Register */
	byte = pci_read_config8(dev, 0x40);
	byte |= 1 << 1;
	pci_write_config8(dev, 0x40, byte);

	/* RPR 4.5 Enables the PCIB to retain ownership of the bus on the Primary side and on the Secondary side when GNT# is deasserted */
	pci_write_config8(dev, 0x0D, 0x40);
	pci_write_config8(dev, 0x1B, 0x40);

	/* RPR 4.6 Enable the command matching checking function on "Memory Read" & "Memory Read Line" commands */
	byte = pci_read_config8(dev, 0x4B);
	byte |= 1 << 6;
	pci_write_config8(dev, 0x4B, byte);

	/* RPR 4.7 When enabled, the PCI arbiter checks for the Bus Idle before asserting GNT# */
	byte = pci_read_config8(dev, 0x4B);
	byte |= 1 << 0;
	pci_write_config8(dev, 0x4B, byte);

	/* RPR 4.8 Adjusts the GNT# de-assertion time */
	word = pci_read_config16(dev, 0x64);
	word |= 1 << 12;
	pci_write_config16(dev, 0x64, word);

	/* RPR 4.9 Fast Back to Back transactions support */
	byte = pci_read_config8(dev, 0x48);
	byte |= 1 << 2;
	pci_write_config8(dev, 0x48, byte);

	/* RPR 4.10 Enable Lock Operation */
	byte = pci_read_config8(dev, 0x48);
	byte |= 1 << 3;
	pci_write_config8(dev, 0x48, byte);
	byte = pci_read_config8(dev, 0x40);
	byte |= (1 << 2);
	pci_write_config8(dev, 0x40, byte);

	/* RPR 4.11 Enable additional optional PCI clock */
	word = pci_read_config16(dev, 0x64);
	word |= 1 << 8;
	pci_write_config16(dev, 0x64, word);

	/* rpr4.12 Disable Fewer-Retry Mode for A11-A13 only. 0x64[5:4] clear */
	byte = pci_read_config8(dev, 0x64);
	byte &= 0xcf;
	pci_write_config8(dev, 0x64, byte);

	/* rpr4.14 Disabling Downstream Flush, for A12 only, 0x64[18]. */
	dword = pci_read_config32(dev, 0x64);
	dword |= (1 << 18);
	pci_write_config32(dev, 0x64, dword);

	/* RPR 4.13 Enable One-Prefetch-Channel Mode */
	dword = pci_read_config32(dev, 0x64);
	dword |= 1 << 20;
	pci_write_config32(dev, 0x64, dword);

	/* RPR 4.15 Disable PCIB MSI Capability */
	byte = pci_read_config8(dev, 0x40);
	byte &= ~(1 << 3);
	pci_write_config8(dev, 0x40, byte);

	/* rpr4.16 Adjusting CLKRUN# */
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
	/* .enable           = sb600_enable, */
	.reset_bus = pci_bus_reset,
	.ops_pci = &lops_pci,
};

static const struct pci_driver pci_driver __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB600_PCI,
};
