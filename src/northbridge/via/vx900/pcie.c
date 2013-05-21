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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>

static void dump_pci_device(device_t dev)
{
	int i;
	for (i = 0; i <= 255; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
			print_debug_hex8(i);
			print_debug_char(':');
		}
		val = pci_read_config8(dev, i);
		if ((i & 7) == 0)
			print_debug(" |");
		print_debug_char(' ');
		print_debug_hex8(val);
		if ((i & 0x0f) == 0x0f) {
			print_debug("\n");
		}
	}
}

static void vx900_pex_dev_set_resources(device_t dev)
{
	//print_debug("======================================================\n");
	//print_debug("== PEX set\n");
	//print_debug("======================================================\n");
	
	assign_resources(dev->link_list);
}

static void vx900_pex_init(device_t dev)
{
	//print_debug("======================================================\n");
	//print_debug("== PEX init\n");
	//print_debug("======================================================\n");
	
	if (0) dump_pci_device(dev);
}

static struct device_operations vx900_pex_ops = {
	.read_resources = pci_bus_read_resources,
	.set_resources = vx900_pex_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init = vx900_pex_init,
	.scan_bus = pciexp_scan_bridge,
	.reset_bus = pci_bus_reset,
};

static const struct pci_driver pex1_driver __pci_driver = {
	.ops = &vx900_pex_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX900_PEX1,
};

static const struct pci_driver pex2_driver __pci_driver = {
	.ops = &vx900_pex_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX900_PEX2,
};

static const struct pci_driver pex3_driver __pci_driver = {
	.ops = &vx900_pex_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX900_PEX3,
};

static const struct pci_driver pex4_driver __pci_driver = {
	.ops = &vx900_pex_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX900_PEX4,
};
