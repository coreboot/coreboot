/*
 * This file is part of the coreboot project.
 *
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#define RES_DEBUG 0

static void setup_resource_map(const u32 *register_values, u32 max)
{
	u32 i;
//	print_debug("setting up resource map....");

	for(i = 0; i < max; i += 3) {
		device_t dev;
		u32 where;
		u32 reg;

		dev = register_values[i] & ~0xff;
		where = register_values[i] & 0xff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(dev, where, reg);
	}
//	print_debug("done.\n");
}


void setup_resource_map_offset(const u32 *register_values, u32 max, u32 offset_pci_dev, u32 offset_io_base)
{
	u32 i;
//	print_debug("setting up resource map offset....");
	for(i = 0; i < max; i += 3) {
		device_t dev;
		u32 where;
		unsigned long reg;
		dev = (register_values[i] & ~0xfff) + offset_pci_dev;
		where = register_values[i] & 0xfff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2] + offset_io_base;
		pci_write_config32(dev, where, reg);
	}
//	print_debug("done.\n");
}

#define RES_PCI_IO 0x10
#define RES_PORT_IO_8 0x22
#define RES_PORT_IO_32 0x20
#define RES_MEM_IO 0x40

void setup_resource_map_x_offset(const u32 *register_values, u32 max, u32 offset_pci_dev, u32 offset_io_base)
{
	u32 i;

#if RES_DEBUG
	print_debug("setting up resource map ex offset....");

#endif

#if RES_DEBUG
	print_debug("\n");
#endif
	for(i = 0; i < max; i += 4) {
#if RES_DEBUG
		printk(BIOS_DEBUG, "%04x: %02x %08x <- & %08x | %08x\n",
			i/4, register_values[i],
			register_values[i+1] + ( (register_values[i]==RES_PCI_IO) ? offset_pci_dev : 0),
			register_values[i+2],
			register_values[i+3] + ( ( (register_values[i] & RES_PORT_IO_32) == RES_PORT_IO_32) ? offset_io_base : 0)
			);
#endif
		switch (register_values[i]) {
		case RES_PCI_IO: //PCI
			{
			device_t dev;
			u32 where;
			u32 reg;
			dev = (register_values[i+1] & ~0xfff) + offset_pci_dev;
			where = register_values[i+1] & 0xfff;
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
			where = register_values[i+1] + offset_io_base;
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
			where = register_values[i+1] + offset_io_base;
			reg = inl(where);
			reg &= register_values[i+2];
			reg |= register_values[i+3];
			outl(reg, where);
			}
			break;
		} // switch


	}

#if RES_DEBUG
	print_debug("done.\n");
#endif
}

void setup_resource_map_x(const u32 *register_values, u32 max)
{
	u32 i;

#if RES_DEBUG
	print_debug("setting up resource map ex offset....");
#endif

#if RES_DEBUG
	print_debug("\n");
#endif
	for(i = 0; i < max; i += 4) {
#if RES_DEBUG
		printk(BIOS_DEBUG, "%04x: %02x %08x <- & %08x | %08x\n",
		i/4, register_values[i],register_values[i+1], register_values[i+2], register_values[i+3]);
#endif
		switch (register_values[i]) {
		case RES_PCI_IO: //PCI
			{
			device_t dev;
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
		} // switch


	}

#if RES_DEBUG
	print_debug("done.\n");
#endif
}

#if 0
static void setup_iob_resource_map(const u32 *register_values, u32 max)
{
	u32 i;

	for(i = 0; i < max; i += 3) {
		u32 where;
		u32 reg;

		where = register_values[i];
		reg = inb(where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		outb(reg, where);
	}
}

static void setup_io_resource_map(const u32 *register_values, u32 max)
{
	u32 i;

	for(i = 0; i < max; i += 3) {
		u32 where;
		u32 reg;

		where = register_values[i];
		reg = inl(where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];

		outl(reg, where);
	}
}
#endif

