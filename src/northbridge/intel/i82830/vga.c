/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Joseph Smith <joe@settoplinux.org>
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
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static void vga_init(device_t dev) {

	printk_info("Starting Graphics Initialization\n");
	pci_dev_init(dev);
	printk_info("Graphics Initialization Complete\n");
	/* Future TV-OUT code will be called from here. */
}

static const struct device_operations vga_operations = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = vga_init,
	.scan_bus         = 0,
	.enable           = 0,
	.ops_pci          = 0,
};

static const struct pci_driver vga_driver __pci_driver = {
	.ops    = &vga_operations,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x3577,
};
