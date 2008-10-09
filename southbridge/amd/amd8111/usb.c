/*
 *
 * This file is part of the coreboot project.
 * Copyright (C) 2004 Linux Networx
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
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
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/smbus.h>
#include <io.h>
#include "amd8111.h"


static void lpci_set_subsystem(struct device * dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x70, 
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

struct device_operations amd8111_usb = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			      .device = PCI_DEVICE_ID_AMD_8111_USB}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = scan_static_bus,
	.phase4_enable_disable           = amd8111_enable,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = NULL,
	.ops_pci          = &lops_pci
};
