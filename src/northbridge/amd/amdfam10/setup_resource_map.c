/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <inttypes.h>
#include <console/console.h>
#include <northbridge/amd/amdfam10/raminit.h>
#include <northbridge/amd/amdfam10/amdfam10.h>

#define RES_DEBUG 0

void setup_resource_map(const u32 *register_values, u32 max)
{
	u32 i;

	for (i = 0; i < max; i += 3) {
		pci_devfn_t dev;
		u32 where;
		u32 reg;

		dev = register_values[i] & ~0xff;
		where = register_values[i] & 0xff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(dev, where, reg);
	}
}


void setup_resource_map_offset(const u32 *register_values, u32 max, u32 offset_pci_dev, u32 offset_io_base)
{
	u32 i;

	for (i = 0; i < max; i += 3) {
		pci_devfn_t dev;
		u32 where;
		unsigned long reg;
		dev = (register_values[i] & ~0xfff) + offset_pci_dev;
		where = register_values[i] & 0xfff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2] + offset_io_base;
		pci_write_config32(dev, where, reg);
	}
}

void setup_resource_map_x_offset(const u32 *register_values, u32 max, u32 offset_pci_dev, u32 offset_io_base)
{
	u32 i;

	if (IS_ENABLED(RES_DEBUG))
		printk(BIOS_DEBUG, "setting up resource map ex offset....\n");

	for (i = 0; i < max; i += 4) {
		if (IS_ENABLED(RES_DEBUG))
			printk(BIOS_DEBUG, "%04x: %02x %08x <- & %08x | %08x\n",
				i/4, register_values[i],
				register_values[i+1] + ((register_values[i]==RES_PCI_IO) ? offset_pci_dev : 0),
				register_values[i+2],
				register_values[i+3] + (((register_values[i] & RES_PORT_IO_32) == RES_PORT_IO_32) ? offset_io_base : 0)
				);
		switch (register_values[i]) {
		case RES_PCI_IO: //PCI
			{
			pci_devfn_t dev;
			u32 where;
			u32 reg;
			dev = (register_values[i+1] & ~0xfff) + offset_pci_dev;
			where = register_values[i+1] & 0xfff;
			reg = pci_read_config32(dev, where);
			if (IS_ENABLED(RES_DEBUG))
				printk(BIOS_SPEW, "WAS: %08x\n", reg);
			reg &= register_values[i+2];
			reg |= register_values[i+3];
			pci_write_config32(dev, where, reg);
			if (IS_ENABLED(RES_DEBUG))
				printk(BIOS_SPEW, "NOW: %08x\n", reg);
			}
			break;
		case RES_PORT_IO_8: // io 8
			{
			u32 where;
			u32 reg;
			where = register_values[i+1] + offset_io_base;
			reg = inb(where);
			if (IS_ENABLED(RES_DEBUG))
				printk(BIOS_SPEW, "WAS: %08x\n", reg);
			reg &= register_values[i+2];
			reg |= register_values[i+3];
			outb(reg, where);
			if (IS_ENABLED(RES_DEBUG))
				printk(BIOS_SPEW, "NOW: %08x\n", reg);
			}
			break;
		case RES_PORT_IO_32:  //io32
			{
			u32 where;
			u32 reg;
			where = register_values[i+1] + offset_io_base;
			reg = inl(where);
			if (IS_ENABLED(RES_DEBUG))
				printk(BIOS_SPEW, "WAS: %08x\n", reg);
			reg &= register_values[i+2];
			reg |= register_values[i+3];
			outl(reg, where);
			if (IS_ENABLED(RES_DEBUG))
				printk(BIOS_SPEW, "NOW: %08x\n", reg);
			}
			break;
		}
	}

	if (IS_ENABLED(RES_DEBUG))
		printk(BIOS_DEBUG, "done.\n");
}

void setup_resource_map_x(const u32 *register_values, u32 max)
{
	u32 i;

	if (IS_ENABLED(RES_DEBUG))
		printk(BIOS_DEBUG, "setting up resource map ex offset....\n");

	for (i = 0; i < max; i += 4) {
		if (IS_ENABLED(RES_DEBUG))
			printk(BIOS_DEBUG, "%04x: %02x %08x <- & %08x | %08x\n",
				i/4, register_values[i],register_values[i+1], register_values[i+2], register_values[i+3]);
		switch (register_values[i]) {
		case RES_PCI_IO: //PCI
			{
			pci_devfn_t dev;
			u32 where;
			u32 reg;
			dev = register_values[i+1] & ~0xff;
			where = register_values[i+1] & 0xff;
			reg = pci_read_config32(dev, where);
			reg &= register_values[i+2];
			reg |= register_values[i+3];
			pci_write_config32(dev, where, reg);
			}
			break;
		case RES_PORT_IO_8: // io 8
			{
			u32 where;
			u32 reg;
			where = register_values[i+1];
			reg = inb(where);
			reg &= register_values[i+2];
			reg |= register_values[i+3];
			outb(reg, where);
			}
			break;
		case RES_PORT_IO_32:  //io32
			{
			u32 where;
			u32 reg;
			where = register_values[i+1];
			reg = inl(where);
			reg &= register_values[i+2];
			reg |= register_values[i+3];
			outl(reg, where);
			}
			break;
		}
	}

	if (IS_ENABLED(RES_DEBUG))
		printk(BIOS_DEBUG, "done.\n");
}
