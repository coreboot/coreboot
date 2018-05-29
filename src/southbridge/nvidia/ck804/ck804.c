/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
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
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "chip.h"

static u32 final_reg;

static struct device *find_lpc_dev(struct device *dev, unsigned devfn)
{
	struct device *lpc_dev;

	lpc_dev = dev_find_slot(dev->bus->secondary, devfn);
	if (!lpc_dev)
		return lpc_dev;

	if ((lpc_dev->vendor != PCI_VENDOR_ID_NVIDIA)
		|| ((lpc_dev->device != PCI_DEVICE_ID_NVIDIA_CK804_LPC)
		&& (lpc_dev->device != PCI_DEVICE_ID_NVIDIA_CK804_PRO)
		&& (lpc_dev->device != PCI_DEVICE_ID_NVIDIA_CK804_SLAVE)))
	{
		u32 id;
		id = pci_read_config32(lpc_dev, PCI_VENDOR_ID);
		if ((id != (PCI_VENDOR_ID_NVIDIA |
		      (PCI_DEVICE_ID_NVIDIA_CK804_LPC << 16)))
		    && (id != (PCI_VENDOR_ID_NVIDIA |
			 (PCI_DEVICE_ID_NVIDIA_CK804_PRO << 16)))
		    && (id != (PCI_VENDOR_ID_NVIDIA |
			 (PCI_DEVICE_ID_NVIDIA_CK804_SLAVE << 16))))
		{
			lpc_dev = 0;
		}
	}

	return lpc_dev;
}

static void ck804_enable(struct device *dev)
{
	struct device *lpc_dev;
	unsigned index = 0, index2 = 0, deviceid, vendorid, devfn;
	u32 reg_old, reg;
	u8 byte;

	struct southbridge_nvidia_ck804_config *conf;
	conf = dev->chip_info;

	if (dev->device == 0x0000) {
		vendorid = pci_read_config32(dev, PCI_VENDOR_ID);
		deviceid = (vendorid >> 16) & 0xffff;
		/* vendorid &= 0xffff; */
	} else {
		/* vendorid = dev->vendor; */
		deviceid = dev->device;
	}

	devfn = (dev->path.pci.devfn) & ~7;
	switch (deviceid) {
	case PCI_DEVICE_ID_NVIDIA_CK804_SM:
		index = 16;
		break;
	case PCI_DEVICE_ID_NVIDIA_CK804_USB:
		devfn -= (1 << 3);
		index = 8;
		break;
	case PCI_DEVICE_ID_NVIDIA_CK804_USB2:
		devfn -= (1 << 3);
		index = 20;
		break;
	case PCI_DEVICE_ID_NVIDIA_CK804_NIC:
		devfn -= (9 << 3);
		index = 10;
		break;
	case PCI_DEVICE_ID_NVIDIA_CK804_NIC_BRIDGE:
		devfn -= (9 << 3);
		index = 10;
		break;
	case PCI_DEVICE_ID_NVIDIA_CK804_ACI:
		devfn -= (3 << 3);
		index = 12;
		break;
	case PCI_DEVICE_ID_NVIDIA_CK804_MCI:
		devfn -= (3 << 3);
		index = 13;
		break;
	case PCI_DEVICE_ID_NVIDIA_CK804_IDE:
		devfn -= (5 << 3);
		index = 14;
		break;
	case PCI_DEVICE_ID_NVIDIA_CK804_SATA0:
		devfn -= (6 << 3);
		index = 22;
		break;
	case PCI_DEVICE_ID_NVIDIA_CK804_SATA1:
		devfn -= (7 << 3);
		index = 18;
		break;
	case PCI_DEVICE_ID_NVIDIA_CK804_PCI:
		devfn -= (8 << 3);
		index = 15;
		break;
	case PCI_DEVICE_ID_NVIDIA_CK804_PCI_E:
		devfn -= (0xa << 3);
		index2 = 19;
		break;
	default:
		index = 0;
	}

	if (index2 != 0) {
		int i;
		for (i = 0; i < 4; i++) {
			lpc_dev = find_lpc_dev(dev, devfn - (i << 3));
			if (!lpc_dev)
				continue;
			index2 -= i;
			break;
		}

		if (lpc_dev) {
			reg_old = reg = pci_read_config32(lpc_dev, 0xe4);
			if (!dev->enabled)
				reg |= (1 << index2);
			if (reg != reg_old)
				pci_write_config32(lpc_dev, 0xe4, reg);
		}

		index2 = 0;
		return;
	}

	lpc_dev = find_lpc_dev(dev, devfn);
	if (!lpc_dev)
		return;

	if (index == 0) {
		final_reg = pci_read_config32(lpc_dev, 0xe8);
		final_reg &= ~((1 << 16) | (1 << 8) | (1 << 20) | (1 << 10)
			| (1 << 12) | (1 << 13) | (1 << 14) | (1 << 22)
			| (1 << 18) | (1 << 15));
		pci_write_config32(lpc_dev, 0xe8, final_reg);

		reg_old = reg = pci_read_config32(lpc_dev, 0xe4);
		reg |= (1 << 20);
		if (reg != reg_old)
			pci_write_config32(lpc_dev, 0xe4, reg);

		byte = pci_read_config8(lpc_dev, 0x74);
		byte |= ((1 << 1));
		pci_write_config8(dev, 0x74, byte);

		byte = pci_read_config8(lpc_dev, 0xdd);
		byte |= ((1 << 0) | (1 << 3));
		pci_write_config8(dev, 0xdd, byte);

		return;
	}

	if (!dev->enabled)
		final_reg |= (1 << index);

	if (index == 10) {
		reg_old = pci_read_config32(lpc_dev, 0xe8);
		if (final_reg != reg_old)
			pci_write_config32(lpc_dev, 0xe8, final_reg);
	}
}

static void ck804_set_subsystem(struct device *dev, unsigned vendor,
				unsigned device)
{
	pci_write_config32(dev, 0x40,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
}

struct pci_operations ck804_pci_ops = {
	.set_subsystem = ck804_set_subsystem,
};

struct chip_operations southbridge_nvidia_ck804_ops = {
	CHIP_NAME("NVIDIA CK804 Southbridge")
	.enable_dev = ck804_enable,
};
