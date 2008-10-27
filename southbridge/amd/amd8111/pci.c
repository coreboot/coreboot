/*
 *
 * This file is part of the coreboot project.
 * Copyright (C) 2003 Linux Networx
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
#include "amd8111.h"

static void pci_init(struct device *dev)
{

	/* Enable pci error detecting */
	u32 dword;

	/* System error enable */
	dword = pci_read_config32(dev, PCI_COMMAND);
	dword |= (1<<8); /* System error enable */
	dword |= (7<<28); /* Clear possible errors */
	pci_write_config32(dev, PCI_COMMAND, dword);

	/* System,Parity,timer,and abort error enable */
	dword = pci_read_config32(dev, 0x3c);
	dword |= (1<<16); /* Parity */
	dword |= (1<<17); /* System */
	dword |= (1<<21); /* Master abort */
//	dword &= ~(1<<21); /* Master abort */
//	dword |= (1<<27); /* Discard timer */
	dword &= ~(1<<27); /* Discard timer */
	dword |= (1<<26); /* DTSTAT error clear  */
	pci_write_config32(dev, 0x3c, dword);

	/* CRC flood enable */
	dword = pci_read_config32(dev, 0xc4);
	dword |= (1<<1); /* CRC Flood enable */
	dword |= (1<<8); /* Clear any CRC errors */
	dword |= (1<<4); /* Clear any LKFAIL errors */
	pci_write_config32(dev, 0xc4, dword);

	/* Clear possible errors */
	dword = pci_read_config32(dev, 0x1c);
	dword |= (1<<27); /* STA */
	dword |= (1<<28); /* RTA */
	dword |= (1<<29); /* RMA */
	dword |= (1<<30); /* RSE */
	dword |= (1<<31); /* DPE */
	dword |= (1<<24); /* MDPE */
	pci_write_config32(dev, 0x1c, dword);
}

struct device_operations amd8111_pci = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			 .device = PCI_DEVICE_ID_AMD_8111_PCI}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = pci_scan_bridge,
	.phase3_chip_setup_dev	 = amd8111_enable,
	.phase4_read_resources	 = pci_bus_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_bus_enable_resources,
	.phase6_init		 = pci_init,
};
