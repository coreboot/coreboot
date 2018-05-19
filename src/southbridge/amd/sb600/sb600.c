/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
#include "sb600.h"

static struct device *find_sm_dev(struct device *dev, u32 devfn)
{
	struct device *sm_dev;

	sm_dev = dev_find_slot(dev->bus->secondary, devfn);
	if (!sm_dev)
		return sm_dev;

	if ((sm_dev->vendor != PCI_VENDOR_ID_ATI) ||
	    ((sm_dev->device != PCI_DEVICE_ID_ATI_SB600_SM))) {
		u32 id;
		id = pci_read_config32(sm_dev, PCI_VENDOR_ID);
		if ((id !=
		     (PCI_VENDOR_ID_ATI | (PCI_DEVICE_ID_ATI_SB600_SM << 16))))
		{
			sm_dev = 0;
		}
	}

	return sm_dev;
}

void set_sm_enable_bits(struct device *sm_dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = pci_read_config32(sm_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pci_write_config32(sm_dev, reg_pos, reg);
	}
}

static void pmio_write_index(u16 port_base, u8 reg, u8 value)
{
	outb(reg, port_base);
	outb(value, port_base + 1);
}

static u8 pmio_read_index(u16 port_base, u8 reg)
{
	outb(reg, port_base);
	return inb(port_base + 1);
}

void pm_iowrite(u8 reg, u8 value)
{
	pmio_write_index(PM_INDEX, reg, value);
}

u8 pm_ioread(u8 reg)
{
	return pmio_read_index(PM_INDEX, reg);
}

void pm2_iowrite(u8 reg, u8 value)
{
	pmio_write_index(PM2_INDEX, reg, value);
}

u8 pm2_ioread(u8 reg)
{
	return pmio_read_index(PM2_INDEX, reg);
}

static void set_pmio_enable_bits(struct device *sm_dev, u32 reg_pos, u32 mask,
				 u32 val)
{
	u8 reg_old, reg;
	reg = reg_old = pm_ioread(reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pm_iowrite(reg_pos, reg);
	}
}

void sb600_enable(struct device *dev)
{
	struct device *sm_dev = NULL;
	struct device *bus_dev = NULL;
	int index = -1;
	u32 deviceid;
	u32 vendorid;

	/* struct southbridge_ati_sb600_config *conf; */
	/* conf = dev->chip_info; */
	int i;

	u32 devfn;

	printk(BIOS_DEBUG, "sb600_enable()\n");

	/*
	 * 0:12.0  SATA	bit 8 of sm_dev 0xac : 1 - enable, default         + 32 * 3
	 * 0:13.1  USB-1	bit 2 of sm_dev 0x68
	 * 0:13.2  USB-2	bit 3 of sm_dev 0x68
	 * 0:13.3  USB-3	bit 4 of sm_dev 0x68
	 * 0:13.4  USB-4	bit 5 of sm_dev 0x68
	 * 0:13.5  USB2	bit 0 of sm_dev 0x68 : 1 - enable, default
	 * 0:14.0  SMBUS							0
	 * 0:14.1  IDE							1
	 * 0:14.2  HDA	bit 3 of pm_io 0x59 : 1 - enable, default	    + 32 * 4
	 * 0:14.3  LPC	bit 20 of sm_dev 0x64 : 0 - disable, default  + 32 * 1
	 * 0:14.4  PCI							4
	 * 0:14.5  ACI	bit 0 of pm_io 0x59 : 0 - enable, default
	 * 0:14.6  MCI	bit 1 of pm_io 0x59 : 0 - enable, default
	 */
	if (dev->device == 0x0000) {
		vendorid = pci_read_config32(dev, PCI_VENDOR_ID);
		deviceid = (vendorid >> 16) & 0xffff;
		vendorid &= 0xffff;
	} else {
		vendorid = dev->vendor;
		deviceid = dev->device;
	}
	bus_dev = dev->bus->dev;
	if ((bus_dev->vendor == PCI_VENDOR_ID_ATI) &&
	    (bus_dev->device == PCI_DEVICE_ID_ATI_SB600_PCI)) {
		devfn = (bus_dev->path.pci.devfn) & ~7;
		sm_dev = find_sm_dev(bus_dev, devfn);
		if (!sm_dev)
			return;

		/* something under 00:01.0 */
		switch (dev->path.pci.devfn) {
		case 5 << 3:
			;
		}

		return;
	}

	i = (dev->path.pci.devfn) & ~7;
	i += (2 << 3);
	for (devfn = (0x14 << 3); devfn <= i; devfn += (1 << 3)) {
		sm_dev = find_sm_dev(dev, devfn);
		if (sm_dev)
			break;
	}
	if (!sm_dev)
		return;

	switch (dev->path.pci.devfn - (devfn - (0x14 << 3))) {
	case (0x12 << 3) | 0:
		index = 8;
		set_sm_enable_bits(sm_dev, 0xac, 1 << index,
				   (dev->enabled ? 1 : 0) << index);
		index += 32 * 3;
		break;
	case (0x13 << 3) | 0:
	case (0x13 << 3) | 1:
	case (0x13 << 3) | 2:
	case (0x13 << 3) | 3:
	case (0x13 << 3) | 4:
	case (0x13 << 3) | 5:
		index = dev->path.pci.devfn & 7;
		index++;
		index %= 6;
		set_sm_enable_bits(sm_dev, 0x68, 1 << index,
				   (dev->enabled ? 1 : 0) << index);
		index += 32 * 2;
		break;
	case (0x14 << 3) | 0:
		index = 0;
		break;
	case (0x14 << 3) | 1:
		index = 1;
		break;
	case (0x14 << 3) | 2:
		index = 3;
		set_pmio_enable_bits(sm_dev, 0x59, 1 << index,
				     (dev->enabled ? 1 : 0) << index);
		index += 32 * 4;
		break;
	case (0x14 << 3) | 3:
		index = 20;
		set_sm_enable_bits(sm_dev, 0x64, 1 << index,
				   (dev->enabled ? 1 : 0) << index);
		index += 32 * 1;
		break;
	case (0x14 << 3) | 4:
		index = 4;
		break;
	case (0x14 << 3) | 5:
	case (0x14 << 3) | 6:
		index = dev->path.pci.devfn & 7;
		index -= 5;
		set_pmio_enable_bits(sm_dev, 0x59, 1 << index,
				     (dev->enabled ? 0 : 1) << index);
		index += 32 * 4;
		break;
	default:
		printk(BIOS_DEBUG, "unknown dev: %s deviceid=%4x\n", dev_path(dev),
			     deviceid);
	}
}

struct chip_operations southbridge_amd_sb600_ops = {
	CHIP_NAME("ATI SB600")
	.enable_dev = sb600_enable,
};
