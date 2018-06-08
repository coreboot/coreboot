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
 */

#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "mcp55.h"

static u32 final_reg;

static struct device *find_lpc_dev(struct device *dev, unsigned devfn)
{
	struct device *lpc_dev;

	lpc_dev = dev_find_slot(dev->bus->secondary, devfn);

	if (!lpc_dev)
		return lpc_dev;

	if ((lpc_dev->vendor != PCI_VENDOR_ID_NVIDIA) || (
		(lpc_dev->device < PCI_DEVICE_ID_NVIDIA_MCP55_LPC) ||
		(lpc_dev->device > PCI_DEVICE_ID_NVIDIA_MCP55_PRO)))
	{
		u32 id;
		id = pci_read_config32(lpc_dev, PCI_VENDOR_ID);
		if ((id < (PCI_VENDOR_ID_NVIDIA
			| (PCI_DEVICE_ID_NVIDIA_MCP55_LPC << 16))) ||
			(id > (PCI_VENDOR_ID_NVIDIA
			| (PCI_DEVICE_ID_NVIDIA_MCP55_PRO << 16))))
		{
			lpc_dev = 0;
		}
	}

	return lpc_dev;
}

void mcp55_enable(struct device *dev)
{
	struct device *lpc_dev = NULL, *sm_dev = NULL;
	unsigned index = 0, index2 = 0;
	u32 reg_old, reg;
	u8 byte;
	unsigned deviceid, vendorid, devfn;
	struct southbridge_nvidia_mcp55_config *conf;
	conf = dev->chip_info;
	int i;

	if (dev->device == 0x0000) {
		vendorid = pci_read_config32(dev, PCI_VENDOR_ID);
		deviceid = (vendorid >> 16) & 0xffff;
	} else {
		deviceid = dev->device;
	}

	devfn = (dev->path.pci.devfn) & ~7;
	switch (deviceid) {
	case PCI_DEVICE_ID_NVIDIA_MCP55_HT:
		return;
	case PCI_DEVICE_ID_NVIDIA_MCP55_SM2: //?
		index = 16;
		break;
	case PCI_DEVICE_ID_NVIDIA_MCP55_USB:
		devfn -= (1 << 3);
		index = 8;
		break;
	case PCI_DEVICE_ID_NVIDIA_MCP55_USB2:
		devfn -= (1 << 3);
		index = 20;
		break;
	case PCI_DEVICE_ID_NVIDIA_MCP55_NIC: // two
	case PCI_DEVICE_ID_NVIDIA_MCP55_NIC_BRIDGE: // two
		devfn -= (7 << 3);
		index = 10;
		for (i = 0; i < 2; i++) {
			lpc_dev = find_lpc_dev(dev, devfn - (i << 3));
			if (!lpc_dev)
				continue;
			index -= i;
			devfn -= (i << 3);
			break;
		}
		break;
	case PCI_DEVICE_ID_NVIDIA_MCP55_AZA:
		devfn -= (5 << 3);
		index = 11;
		break;
	case PCI_DEVICE_ID_NVIDIA_MCP55_IDE:
		devfn -= (3 << 3);
		index = 14;
		break;
	case PCI_DEVICE_ID_NVIDIA_MCP55_SATA0: // three
	case PCI_DEVICE_ID_NVIDIA_MCP55_SATA1: // three
		devfn -= (4 << 3);
		index = 22;
		i = (dev->path.pci.devfn) & 7;
		if (i > 0)
			index -= (i + 3);
		break;
	case PCI_DEVICE_ID_NVIDIA_MCP55_PCI:
		devfn -= (5 << 3);
		index = 15;
		break;
	case PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_A:
		devfn -= (0x9 << 3); // to LPC
		index2 = 9;
		break;
	case PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_B_C: // two
		devfn -= (0xa << 3);  // to LPC
		index2 = 8;
		for (i = 0; i < 2; i++) {
			lpc_dev = find_lpc_dev(dev, devfn - (i << 3));
			if (!lpc_dev)
				continue;
			index2 -= i;
			devfn -= (i << 3);
			break;
		}
		break;
	case PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_D:
		devfn -= (0xc << 3); // to LPC
		index2 = 6;
		break;
	case PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_E:
		devfn -= (0xd << 3); // to LPC
		index2 = 5;
		break;
	case PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_F:
		devfn -= (0xe << 3); // to LPC
		index2 = 4;
		break;
	default:
		index = 0;
	}

	if (!lpc_dev)
		lpc_dev = find_lpc_dev(dev, devfn);

	if (!lpc_dev)
		return;

	if (index2 != 0) {
		sm_dev = dev_find_slot(dev->bus->secondary, devfn + 1);
		if (!sm_dev)
			return;
		if (sm_dev) {
			reg_old = reg = pci_read_config32(sm_dev, 0xe4);
			if (!dev->enabled)
				reg |= (1<<index2); /* Disable it. */
			if (reg != reg_old)
				pci_write_config32(sm_dev, 0xe4, reg);
		}
		index2 = 0;
		return;
	}

	if (index == 0) { // for LPC
		/* Expose IOAPIC base. */
		byte = pci_read_config8(lpc_dev, 0x74);
		byte |= (1 << 1); /* Expose the BAR. */
		pci_write_config8(dev, 0x74, byte);

		/* Expose trap base. */
		byte = pci_read_config8(lpc_dev, 0xdd);
		byte |= (1 << 0) | (1 << 3); /* Expose BAR and enable write. */
		pci_write_config8(dev, 0xdd, byte);

		return;
	}

	if (index == 16) {
		sm_dev = dev_find_slot(dev->bus->secondary, devfn + 1);
		if (!sm_dev)
			return;

		final_reg = pci_read_config32(sm_dev, 0xe8);
		final_reg &= ~((1 << 16) | (1 << 8) | (1 << 20) | (1 << 14)
				| (1 << 22) | (1 << 18) | (1 << 17) | (1 << 15)
				| (1 << 11) | (1 << 10) | (1 << 9));
		pci_write_config32(sm_dev, 0xe8, final_reg); /* Enable all at first. */

	}

	if (!dev->enabled) {
		final_reg |= (1 << index); /* Disable it. */
		/*
		 * The reason for using final_reg is that if func 1 is disabled,
		 * then func 2 will become func 1.
		 * Because of this, we need loop through disabling them all at
		 * the same time.
		 */
	}

	/* NIC1 is the final, we need update final reg to 0xe8. */
	if (index == 9) {
		sm_dev = dev_find_slot(dev->bus->secondary, devfn + 1);
		if (!sm_dev)
			return;
		reg_old = pci_read_config32(sm_dev, 0xe8);
		if (final_reg != reg_old)
			pci_write_config32(sm_dev, 0xe8, final_reg);
	}
}

static void mcp55_set_subsystem(struct device *dev, unsigned vendor,
				unsigned device)
{
	pci_write_config32(dev, 0x40,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
}

struct pci_operations mcp55_pci_ops = {
	.set_subsystem = mcp55_set_subsystem,
};

struct chip_operations southbridge_nvidia_mcp55_ops = {
	CHIP_NAME("NVIDIA MCP55 Southbridge")
	.enable_dev = mcp55_enable,
};
