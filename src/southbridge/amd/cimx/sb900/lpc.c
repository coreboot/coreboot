/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#include <device/pci.h>
#include "lpc.h"
#include <console/console.h>	/* printk */


void lpc_read_resources(device_t dev)
{
	struct resource *res;

	printk(BIOS_DEBUG, "SB900 - Lpc.c - lpc_read_resources - Start.\n");
	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);	/* We got one for APIC, or one more for TRAP */

	pci_get_resource(dev, SPIROM_BASE_ADDRESS); /* SPI ROM base address */

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = 0xff800000;
	res->size = 0x00800000; /* 8 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = 0xfec00000;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	compact_resources(dev);
	printk(BIOS_DEBUG, "SB900 - Lpc.c - lpc_read_resources - End.\n");
}

void lpc_set_resources(struct device *dev)
{
	struct resource *res;

	printk(BIOS_DEBUG, "SB900 - Lpc.c - lpc_set_resources - Start.\n");
	pci_dev_set_resources(dev);

	/* Specical case. SPI Base Address. The SpiRomEnable should be set. */
	res = find_resource(dev, SPIROM_BASE_ADDRESS);
	pci_write_config32(dev, SPIROM_BASE_ADDRESS, res->base | 1 << 1);
	printk(BIOS_DEBUG, "SB900 - Lpc.c - lpc_set_resources - End.\n");
}

/**
 * @brief Enable resources for children devices
 *
 * @param dev the device whos children's resources are to be enabled
 *
 */
void lpc_enable_childrens_resources(device_t dev)
{
	struct bus *link;
	u32 reg, reg_x;
	int var_num = 0;
	u16 reg_var[3];

	printk(BIOS_DEBUG, "SB900 - Lpc.c - lpc_enable_childrens_resources - Start.\n");
	reg = pci_read_config32(dev, 0x44);
	reg_x = pci_read_config32(dev, 0x48);

	for (link = dev->link_list; link; link = link->next) {
		device_t child;
		for (child = link->children; child;
		     child = child->sibling) {
			if (child->enabled
			    && (child->path.type == DEVICE_PATH_PNP)) {
				struct resource *res;
				for (res = child->resource_list; res; res = res->next) {
					u32 base, end;	/*  don't need long long */
					if (!(res->flags & IORESOURCE_IO))
						continue;
					base = res->base;
					end = resource_end(res);
/*
					printk(BIOS_DEBUG, "sb900 lpc decode:%s, base=0x%08x, end=0x%08x\n",
					     dev_path(child), base, end);
*/
					switch (base) {
					case 0x60:	/*  KB */
					case 0x64:	/*  MS */
						reg |= (1 << 29);
						break;
					case 0x3f8:	/*  COM1 */
						reg |= (1 << 6);
						break;
					case 0x2f8:	/*  COM2 */
						reg |= (1 << 7);
						break;
					case 0x378:	/*  Parallal 1 */
						reg |= (1 << 0);
						break;
					case 0x3f0:	/*  FD0 */
						reg |= (1 << 26);
						break;
					case 0x220:	/*  Aduio 0 */
						reg |= (1 << 8);
						break;
					case 0x300:	/*  Midi 0 */
						reg |= (1 << 18);
						break;
					case 0x400:
						reg_x |= (1 << 16);
						break;
					case 0x480:
						reg_x |= (1 << 17);
						break;
					case 0x500:
						reg_x |= (1 << 18);
						break;
					case 0x580:
						reg_x |= (1 << 19);
						break;
					case 0x4700:
						reg_x |= (1 << 22);
						break;
					case 0xfd60:
						reg_x |= (1 << 23);
						break;
					default:
						if (var_num >= 3)
							continue;	/* only 3 var ; compact them ? */
						switch (var_num) {
						case 0:
							reg_x |= (1 << 2);
							break;
						case 1:
							reg_x |= (1 << 24);
							break;
						case 2:
							reg_x |= (1 << 25);
							break;
						}
						reg_var[var_num++] =
						    base & 0xffff;
					}
				}
			}
		}
	}
	pci_write_config32(dev, 0x44, reg);
	pci_write_config32(dev, 0x48, reg_x);
	/* Set WideIO for as many IOs found (fall through is on purpose) */
	switch (var_num) {
	case 2:
		pci_write_config16(dev, 0x90, reg_var[2]);
	case 1:
		pci_write_config16(dev, 0x66, reg_var[1]);
	case 0:
		//pci_write_config16(dev, 0x64, reg_var[0]); //cause filo can not find sata
		break;
	}
	printk(BIOS_DEBUG, "SB900 - Lpc.c - lpc_enable_childrens_resources - End.\n");
}
