/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>

#include <arch/io.h>

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "sb700.h"

static device_t find_sm_dev(device_t dev, u32 devfn)
{
	device_t sm_dev;

	sm_dev = dev_find_slot(dev->bus->secondary, devfn);
	if (!sm_dev)
		return sm_dev;

	if ((sm_dev->vendor != PCI_VENDOR_ID_ATI) ||
	    ((sm_dev->device != PCI_DEVICE_ID_ATI_SB700_SM))) {
		u32 id;
		id = pci_read_config32(sm_dev, PCI_VENDOR_ID);
		if ((id !=
		     (PCI_VENDOR_ID_ATI | (PCI_DEVICE_ID_ATI_SB700_SM << 16))))
		{
			sm_dev = 0;
		}
	}

	return sm_dev;
}

void set_sm_enable_bits(device_t sm_dev, u32 reg_pos, u32 mask, u32 val)
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

static void set_pmio_enable_bits(device_t sm_dev, u32 reg_pos,
				 u32 mask, u32 val)
{
	u8 reg_old, reg;
	reg = reg_old = pm_ioread(reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pm_iowrite(reg_pos, reg);
	}
}

void sb7xx_51xx_enable(device_t dev)
{
	device_t sm_dev = 0;
	device_t bus_dev = 0;
	int index = -1;
	u32 deviceid;
	u32 vendorid;

	/* struct southbridge_ati_sb700_config *conf; */
	/* conf = dev->chip_info; */
	int i;

	u32 devfn;

	printk(BIOS_DEBUG, "sb7xx_51xx_enable()\n");

	/*
	 * 0:11.0  SATA	bit 8 of sm_dev 0xac : 1 - enable, default         + 32 * 3
	 * 0:12.0  OHCI0-USB1	bit 0 of sm_dev 0x68
	 * 0:12.1  OHCI1-USB1	bit 1 of sm_dev 0x68
	 * 0:12.2  EHCI-USB1	bit 2 of sm_dev 0x68
	 * 0:13.0  OHCI0-USB2	bit 4 of sm_dev 0x68
	 * 0:13.1  OHCI1-USB2	bit 5 of sm_dev 0x68
	 * 0:13.2  EHCI-USB2	bit 6 of sm_dev 0x68
	 * 0:14.5  OHCI0-USB3	bit 7 of sm_dev 0x68
	 * 0:14.0  SMBUS							0
	 * 0:14.1  IDE							1
	 * 0:14.2  HDA	bit 3 of pm_io 0x59 : 1 - enable, default	    + 32 * 4
	 * 0:14.3  LPC	bit 20 of sm_dev 0x64 : 0 - disable, default  + 32 * 1
	 * 0:14.4  PCI							4
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
	    (bus_dev->device == PCI_DEVICE_ID_ATI_SB700_PCI)) {
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
	i += (3 << 3);
	for (devfn = (0x14 << 3); devfn <= i; devfn += (1 << 3)) {
		sm_dev = find_sm_dev(dev, devfn);
		if (sm_dev)
			break;
	}
	if (!sm_dev)
		return;

	switch (dev->path.pci.devfn - (devfn - (0x14 << 3))) {
	case (0x11 << 3) | 0:
		index = 8;
		set_sm_enable_bits(sm_dev, 0xac, 1 << index,
				   (dev->enabled ? 1 : 0) << index);
		index += 32 * 3;
		break;
	case (0x12 << 3) | 0:
	case (0x12 << 3) | 1:
	case (0x12 << 3) | 2:
		index = dev->path.pci.devfn & 3;
		set_sm_enable_bits(sm_dev, 0x68, 1 << index,
				   (dev->enabled ? 1 : 0) << index);
		index += 32 * 2;
		break;
	case (0x13 << 3) | 0:
	case (0x13 << 3) | 1:
	case (0x13 << 3) | 2:
		index = (dev->path.pci.devfn & 3) + 4;
		set_sm_enable_bits(sm_dev, 0x68, 1 << index,
				   (dev->enabled ? 1 : 0) << index);
		index += 32 * 2;
		break;
	case (0x14 << 3) | 5:
		index = 7;
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
	default:
		printk(BIOS_DEBUG, "unknown dev: %s deviceid=%4x\n", dev_path(dev),
			     deviceid);
	}
}

#if CONFIG_SOUTHBRIDGE_AMD_SUBTYPE_SP5100
struct chip_operations southbridge_amd_sb700_ops = {
	CHIP_NAME("ATI SP5100")
	.enable_dev = sb7xx_51xx_enable,
};
#else
struct chip_operations southbridge_amd_sb700_ops = {
	CHIP_NAME("ATI SB700")
	.enable_dev = sb7xx_51xx_enable,
};
#endif
