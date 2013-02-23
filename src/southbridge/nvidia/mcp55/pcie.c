/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "mcp55.h"

static void pcie_init(struct device *dev)
{
	/* Enable pci error detecting */
	u32 dword;

	/* System error enable */
	dword = pci_read_config32(dev, 0x04);
	dword |= (1<<8); /* System error enable */
	dword |= (1<<30); /* Clear possible errors */
	pci_write_config32(dev, 0x04, dword);

}

static struct device_operations pcie_ops  = {
	.read_resources	= pci_bus_read_resources,
	.set_resources	= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init		= pcie_init,
	.scan_bus	= pci_scan_bridge,
//	.enable		= mcp55_enable,
};

static const unsigned short pcie_ids[] = {
	PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_B_C,
	PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_E,
	PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_A,
	PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_F,
	PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_D,
	0
};

static const struct pci_driver pciebc_driver __pci_driver = {
	.ops	= &pcie_ops,
	.vendor	= PCI_VENDOR_ID_NVIDIA,
	.devices= pcie_ids,
};
