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

#include "amdk8.h"

#define RES_DEBUG 0

void setup_resource_map_offset(const unsigned int *register_values, int max, unsigned offset_pci_dev, unsigned offset_io_base)
{
	int i;
#if RES_DEBUG
	printk(BIOS_DEBUG, "setting up resource map offset....\n");
#endif
	for (i = 0; i < max; i += 3) {
		pci_devfn_t dev;
		unsigned where;
		unsigned long reg = 0;
#if RES_DEBUG
		prink_debug("%08x <- %08x\n", register_values[i] +  offset_pci_dev, register_values[i+2]);
#endif
		dev = (register_values[i] & ~0xfff) + offset_pci_dev;
		where = register_values[i] & 0xfff;
		if (register_values[i+1])
			reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2] + offset_io_base;
		pci_write_config32(dev, where, reg);
	}
#if RES_DEBUG
	printk(BIOS_DEBUG, "done.\n");
#endif
}

#define RES_PCI_IO 0x10
#define RES_PORT_IO_8 0x22
#define RES_PORT_IO_32 0x20
#define RES_MEM_IO 0x40

static void setup_resource_map_x_offset(const unsigned int *register_values, int max, unsigned offset_pci_dev, unsigned offset_io_base)
{
	int i;

#if RES_DEBUG
	printk(BIOS_DEBUG, "setting up resource map ex offset....\n");
#endif
	for (i = 0; i < max; i += 4) {
#if RES_DEBUG
		printk(BIOS_DEBUG, "%04x: %02x %08x <- & %08x | %08x\n",
			i>>2, register_values[i],
			register_values[i+1] + ((register_values[i]==RES_PCI_IO) ? offset_pci_dev : 0),
			register_values[i+2],
			register_values[i+3] + (((register_values[i] & RES_PORT_IO_32) == RES_PORT_IO_32) ? offset_io_base : 0)
			);
#endif
		switch (register_values[i]) {
		case RES_PCI_IO: //PCI
			{
			pci_devfn_t dev;
			unsigned where;
			unsigned long reg = 0;
			dev = (register_values[i+1] & ~0xfff) + offset_pci_dev;
			where = register_values[i+1] & 0xfff;
			if (register_values[i+2])
				reg = pci_read_config32(dev, where);
			reg &= register_values[i+2];
			reg |= register_values[i+3];
			pci_write_config32(dev, where, reg);
			}
			break;
		case RES_PORT_IO_8: // io 8
			{
			unsigned where;
			unsigned reg = 0;
			where = register_values[i+1] + offset_io_base;
			if (register_values[i+2])
				reg = inb(where);
			reg &= register_values[i+2];
			reg |= register_values[i+3];
			outb(reg, where);
			}
			break;
		case RES_PORT_IO_32:  //io32
			{
			unsigned where;
			unsigned long reg = 0;
			where = register_values[i+1] + offset_io_base;
			if (register_values[i+2])
				reg = inl(where);
			reg &= register_values[i+2];
			reg |= register_values[i+3];
			outl(reg, where);
			}
			break;
		} // switch


	}

#if RES_DEBUG
	printk(BIOS_DEBUG, "done.\n");
#endif
}

static inline void setup_resource_map_x(const unsigned int *register_values, int max)
{
	setup_resource_map_x_offset(register_values, max, 0, 0);
}
