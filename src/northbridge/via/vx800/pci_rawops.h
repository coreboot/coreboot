/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
 * Copyright (C) 2010 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef NORTHBRIDGE_VIA_VX800_PCI_RAWOPS_H
#define NORTHBRIDGE_VIA_VX800_PCI_RAWOPS_H

#include <stdint.h>
#include <arch/io.h>

struct VIA_PCI_REG_INIT_TABLE {
	u8 ChipRevisionStart;
	u8 ChipRevisionEnd;
	u8 Bus;
	u8 Device;
	u8 Function;
	u32 Register;
	u8 Mask;
	u8 Value;
};

static void pci_modify_config8(device_t dev, unsigned where, u8 orval, u8 mask)
{
	u8 data = pci_read_config8(dev, where);
	data &= (~mask);
	data |= orval;
	pci_write_config8(dev, where, data);
}

static void via_pci_inittable(u8 chipversion,
		       const struct VIA_PCI_REG_INIT_TABLE *initdata)
{
	u8 i = 0;
	device_t devbxdxfx;
	for (i = 0;; i++) {
		if ((initdata[i].Mask == 0) && (initdata[i].Value == 0)
		    && (initdata[i].Bus == 0)
		    && (initdata[i].ChipRevisionEnd == 0xff)
		    && (initdata[i].ChipRevisionStart == 0)
		    && (initdata[i].Device == 0)
		    && (initdata[i].Function == 0)
		    && (initdata[i].Register == 0))
			break;
		if ((chipversion >= initdata[i].ChipRevisionStart)
		    && (chipversion <= initdata[i].ChipRevisionEnd)) {
			devbxdxfx =
			    PCI_DEV(initdata[i].Bus, initdata[i].Device,
				       initdata[i].Function);
			pci_modify_config8(devbxdxfx,
					      initdata[i].Register,
					      initdata[i].Value,
					      initdata[i].Mask);
		}
	}
}
#endif
