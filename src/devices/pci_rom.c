/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Li-Ta Lo <ollie@lanl.gov>
 * Copyright (C) 2005 Tyan
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan)
 * Copyright (C) 2005 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2005-2007 Stefan Reinauer <stepan@openbios.org>
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
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <string.h>
#include <cbfs.h>

struct rom_header * pci_rom_probe(struct device *dev)
{
	unsigned long rom_address = 0;
	struct rom_header *rom_header;
	struct pci_data *rom_data;

	if (CONFIG_CBFS) {
		void *v;
		/* if it's in FLASH, then it's as if dev->on_mainboard was true */
		v = cbfs_load_optionrom(dev->vendor, dev->device, NULL);
		printk_debug("In cbfs, rom address for %s = %lx\n", 
				dev_path(dev), rom_address);
		if (v) {
			dev->rom_address = (u32)v;
			dev->on_mainboard = 1;
		}
	} 

	if (dev->on_mainboard) {
		/* this is here as a legacy path. We hope it goes away soon. Users should not have to 
		 * compute the ROM address at build time!
		 */
                // in case some device PCI_ROM_ADDRESS can not be set or readonly 
		rom_address = dev->rom_address;
		printk_debug("On mainboard, rom address for %s = %lx\n", 
			dev_path(dev), rom_address);
        } else {
		rom_address = pci_read_config32(dev, PCI_ROM_ADDRESS);
		printk_debug("On card, rom address for %s = %lx\n", 
				dev_path(dev), rom_address);
	}

	if (rom_address == 0x00000000 || rom_address == 0xffffffff) {
		return NULL;
	}

	if(!dev->on_mainboard) {
		/* enable expansion ROM address decoding */
		pci_write_config32(dev, PCI_ROM_ADDRESS,
				   rom_address|PCI_ROM_ADDRESS_ENABLE);
	}

	rom_header = (struct rom_header *)rom_address;
	printk_spew("PCI Expansion ROM, signature 0x%04x, INIT size 0x%04x, data ptr 0x%04x\n",
		    le32_to_cpu(rom_header->signature),
		    rom_header->size * 512, le32_to_cpu(rom_header->data));
	if (le32_to_cpu(rom_header->signature) != PCI_ROM_HDR) {
		printk_err("Incorrect Expansion ROM Header Signature %04x\n",
			   le32_to_cpu(rom_header->signature));
		return NULL;
	}

	rom_data = (struct pci_data *) ((void *)rom_header + le32_to_cpu(rom_header->data));
	printk_spew("PCI ROM Image, Vendor %04x, Device %04x,\n",
		    rom_data->vendor, rom_data->device);
	if (dev->vendor != rom_data->vendor || dev->device != rom_data->device) {
		printk_err("Device or Vendor ID mismatch Vendor %04x, Device %04x\n",
			   rom_data->vendor, rom_data->device);
		return NULL;
	}

	printk_spew("PCI ROM Image,  Class Code %04x%02x, Code Type %02x\n",
		    rom_data->class_hi, rom_data->class_lo,
		    rom_data->type);
	if (dev->class != ((rom_data->class_hi << 8) | rom_data->class_lo)) {
		printk_debug("Class Code mismatch ROM %08x, dev %08x\n", 
			    (rom_data->class_hi << 8) | rom_data->class_lo, dev->class);
		//return NULL;
	}

	return rom_header;
}

static void *pci_ram_image_start = (void *)PCI_RAM_IMAGE_START;

struct rom_header *pci_rom_load(struct device *dev, struct rom_header *rom_header)
{
	struct pci_data * rom_data;
	unsigned long rom_address;
	unsigned int rom_size;
	unsigned int image_size=0;

	rom_address = pci_read_config32(dev, PCI_ROM_ADDRESS);

	do {
		rom_header = (struct rom_header *)((void *) rom_header + image_size); // get next image
	        rom_data = (struct pci_data *)((void *) rom_header + le32_to_cpu(rom_header->data));
        	image_size = le32_to_cpu(rom_data->ilen) * 512;
	} while ((rom_data->type!=0) && (rom_data->indicator!=0));  // make sure we got x86 version

	if(rom_data->type!=0) return NULL;

	rom_size = rom_header->size * 512;

	if (PCI_CLASS_DISPLAY_VGA == rom_data->class_hi) {
#if CONFIG_CONSOLE_VGA == 1 && CONFIG_CONSOLE_VGA_MULTI == 0
		extern device_t vga_pri;	// the primary vga device, defined in device.c
		if (dev != vga_pri) return NULL; // only one VGA supported
#endif
		printk_debug("copying VGA ROM Image from %p to 0x%x, 0x%x bytes\n",
			    rom_header, PCI_VGA_RAM_IMAGE_START, rom_size);
		memcpy((void *)PCI_VGA_RAM_IMAGE_START, rom_header, rom_size);
		return (struct rom_header *) (PCI_VGA_RAM_IMAGE_START);
	} else {
		printk_debug("copying non-VGA ROM Image from %p to %p, 0x%x bytes\n",
			    rom_header, pci_ram_image_start, rom_size);
		memcpy(pci_ram_image_start, rom_header, rom_size);
		pci_ram_image_start += rom_size;
		return (struct rom_header *) (pci_ram_image_start-rom_size);
	}
	/* disable expansion ROM address decoding */
	pci_write_config32(dev, PCI_ROM_ADDRESS, rom_address & ~PCI_ROM_ADDRESS_ENABLE);
	
	return NULL;
}
