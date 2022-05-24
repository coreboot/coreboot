/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <arch/ioapic.h>
#include "lpc.h"
#include <device/pci_ops.h>

void lpc_read_resources(struct device *dev)
{
	struct resource *res;

	printk(BIOS_DEBUG, "SB800 - Lpc.c - %s - Start.\n", __func__);
	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);	/* We got one for APIC, or one more for TRAP */

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

	/* Add a memory resource for the SPI BAR. */
	fixed_mem_resource_kb(dev, 2, SPI_BASE_ADDRESS / 1024, 1, IORESOURCE_SUBTRACTIVE);

	res = new_resource(dev, 3);
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	compact_resources(dev);
	printk(BIOS_DEBUG, "SB800 - Lpc.c - %s - End.\n", __func__);
}

void lpc_set_resources(struct device *dev)
{
	struct resource *res;

	printk(BIOS_DEBUG, "SB800 - Lpc.c - %s - Start.\n", __func__);

	/* Special case. SPI Base Address. The SpiRomEnable should STAY set. */
	res = find_resource(dev, 2);
	pci_write_config32(dev, SPIROM_BASE_ADDRESS_REGISTER, res->base | SPI_ROM_ENABLE);

	pci_dev_set_resources(dev);

	printk(BIOS_DEBUG, "SB800 - Lpc.c - %s - End.\n", __func__);
}

/**
 * @brief Enable resources for children devices
 *
 * @param dev the device whose children's resources are to be enabled
 *
 */
void lpc_enable_childrens_resources(struct device *dev)
{
	struct bus *link;
	u32 reg, reg_x;
	int var_num = 0;
	u16 reg_var[3];

	printk(BIOS_DEBUG, "SB800 - Lpc.c - %s - Start.\n", __func__);
	reg = pci_read_config32(dev, 0x44);
	reg_x = pci_read_config32(dev, 0x48);

	for (link = dev->link_list; link; link = link->next) {
		struct device *child;
		for (child = link->children; child;
		     child = child->sibling) {
			if (child->enabled
			    && (child->path.type == DEVICE_PATH_PNP)) {
				struct resource *res;
				for (res = child->resource_list; res; res = res->next) {
					u32 base; /* don't need long long */
					if (!(res->flags & IORESOURCE_IO))
						continue;
					base = res->base;
/*
					printk(BIOS_DEBUG, "sb800 lpc decode:%s,
					       base=0x%08x, end=0x%08x\n",
					       dev_path(child), base,
					       resource_end(res));
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
					case 0x378:	/*  Parallel 1 */
						reg |= (1 << 0);
						break;
					case 0x3f0:	/*  FD0 */
						reg |= (1 << 26);
						break;
					case 0x220:	/*  Audio 0 */
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
	case 3:
		pci_write_config16(dev, 0x90, reg_var[2]);
		__fallthrough;
	case 2:
		pci_write_config16(dev, 0x66, reg_var[1]);
		__fallthrough;
	case 1:
		//pci_write_config16(dev, 0x64, reg_var[0]); //cause filo can not find sata
		break;
	}
	printk(BIOS_DEBUG, "SB800 - Lpc.c - %s - End.\n", __func__);
}
