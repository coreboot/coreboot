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

/* This code should work for all ICH* southbridges with a NIC. */

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <config.h>

static void nic_init(struct device *dev)
{
	printk(BIOS_DEBUG, "Initializing RTL8168 Gigabit Ethernet\n");
	// Nothing to do yet, but this has to be here to keep 
	// coreboot from trying to execute an option ROM.
}

struct device_operations nic_ops = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = 0x10ec,
			      .device = 0x8168}}},
	.constructor		 = default_device_constructor,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = nic_init,
	.ops_pci		 = &pci_dev_ops_pci,
};


