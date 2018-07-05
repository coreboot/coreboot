/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "amd8111.h"

static void pci_init(struct device *dev)
{

	/* Enable pci error detecting */
	uint32_t dword;

	/* System error enable */
	dword = pci_read_config32(dev, 0x04);
	dword |= (1<<8); /* System error enable */
	dword |= (7<<28); /* Clear possible errors */
	pci_write_config32(dev, 0x04, dword);

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

static struct pci_operations lops_pci = {
	.set_subsystem = 0,
};

static struct device_operations pci_ops  = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = pci_init,
	.scan_bus         = pci_scan_bridge,
	/* PCI Subordinate bus reset is not implemented */
	.ops_pci          = &lops_pci,
};

static const struct pci_driver pci_driver __pci_driver = {
	.ops    = &pci_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_8111_PCI,
};
