/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
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

#include <types.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <keyboard.h>

static void setup_onboard(struct device *dev)
{
	struct pc_keyboard conf;

	printk(BIOS_INFO, "Init VGA device\n");
	dev->on_mainboard = 1;
	dev->rom_address = 0xc0000;

	// FIXME - this should be in superio some day
	// but since qemu has no superio.
	init_pc_keyboard(0x60, 0x64, &conf);
}

static struct device_operations qemuvga_pci_ops_dev = {
	.constructor			= default_device_constructor,
	.phase3_scan			= 0,
	.phase4_read_resources		= pci_dev_read_resources,
	.phase4_set_resources		= pci_dev_set_resources,
	.phase4_enable_disable		= setup_onboard,
	.phase5_enable_resources	= pci_dev_enable_resources,
	.phase6_init			= pci_dev_init,
	.ops_pci			= &pci_dev_ops_pci,
};

struct constructor qemuvga_constructors[] = {
	{.id = {.type = DEVICE_ID_PCI,
		.u = {.pci = {.vendor = PCI_VENDOR_ID_CIRRUS,
			      .device = PCI_DEVICE_ID_CIRRUS_5446}}},
	 &qemuvga_pci_ops_dev},

	{.ops = 0},
};
