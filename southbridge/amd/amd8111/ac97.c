/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Linux NetworX
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
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include "amd8111.h"

static void lpci_set_subsystem(struct device * dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID, 
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

struct device_operations ac97audio = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			      .device = 0x746D}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = 0,
	.phase3_chip_setup_dev           = amd8111_enable,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = NULL,
	.ops_pci		 = &lops_pci,
};

struct device_operations ac97modem = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			      .device = 0x746E}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = 0,
	.phase3_chip_setup_dev           = amd8111_enable,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = NULL,
	.ops_pci		 = &lops_pci,
};

