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

struct device_operations mcp55_pcie_a = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_A}}},
	.constructor		 = default_device_constructor,
	.reset_bus		= pci_bus_reset,
	.phase3_scan		 = pci_scan_bridge,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_bus_enable_resources,
	.phase6_init		 = pcie_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};

struct device_operations mcp55_pcie_b_c = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_B_C}}},
	.constructor		 = default_device_constructor,
	.reset_bus		= pci_bus_reset,
	.phase3_scan		 = pci_scan_bridge,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_bus_enable_resources,
	.phase6_init		 = pcie_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};

struct device_operations mcp55_pcie_d = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_D}}},
	.constructor		 = default_device_constructor,
	.reset_bus		= pci_bus_reset,
	.phase3_scan		 = pci_scan_bridge,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_bus_enable_resources,
	.phase6_init		 = pcie_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};

struct device_operations mcp55_pcie_e = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_D}}},
	.constructor		 = default_device_constructor,
	.reset_bus		= pci_bus_reset,
	.phase3_scan		 = pci_scan_bridge,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_bus_enable_resources,
	.phase6_init		 = pcie_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};

struct device_operations mcp55_pcie_f = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_F}}},
	.constructor		 = default_device_constructor,
	.reset_bus		= pci_bus_reset,
	.phase3_scan		 = pci_scan_bridge,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_bus_enable_resources,
	.phase6_init		 = pcie_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};
