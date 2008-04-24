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

#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <string.h>
#include <lar.h>

struct rom_header *pci_rom_probe(struct device *dev)
{
	unsigned long rom_address;
	struct rom_header *rom_header;
	struct pci_data *rom_data;
	unsigned int i;
	unsigned char sum = 0, *rom_bytes;
	struct mem_file archive, result;

	if (dev->on_mainboard) {
		int ret;
		char pcifile[] = "pci0000,0000.rom";
		/* In case some device PCI_ROM_ADDRESS can not be set
		 * or readonly.
		 */
		init_archive(&archive);
		sprintf(pcifile, "pci%04x,%04x.rom", dev->id.u.pci.vendor,
				dev->id.u.pci.device);

		ret = find_file(&archive, pcifile, &result);
		if (ret) {
			printk(BIOS_INFO, "No option rom for onboard device.\n");
			return NULL;
		}

		/* FIXME hardcode to 0xc0000 for now because we can only init
		 * VGA anyways.
		 */
		process_file(&result, (void *)0xc0000);
		rom_address = 0xc0000;

	} else {
		if (dev->rom_address) {
			/* Override in place? */
			rom_address = dev->rom_address;
		} else {
			rom_address = pci_read_config32(dev, PCI_ROM_ADDRESS) & 0xfffffffe;
		}

	}

	if (rom_address == 0x00000000 || rom_address == 0xffffffff) {
		return NULL;
	}

	printk(BIOS_DEBUG, "ROM address for %s = %lx\n", dev_path(dev),
	       rom_address);

	if (!dev->on_mainboard) {
		/* Enable expansion ROM address decoding. */
		pci_write_config32(dev, PCI_ROM_ADDRESS,
				   rom_address | PCI_ROM_ADDRESS_ENABLE);
	}

	rom_header = (struct rom_header *)rom_address;
	printk(BIOS_SPEW,
	       "PCI Expansion ROM, signature 0x%04x, INIT size 0x%04x, data ptr 0x%04x\n",
	       le32_to_cpu(rom_header->signature), rom_header->size * 512,
	       le32_to_cpu(rom_header->data));
	if (le32_to_cpu(rom_header->signature) != PCI_ROM_HDR) {
		printk(BIOS_ERR,
		       "Incorrect Expansion ROM Header Signature %04x\n",
		       le32_to_cpu(rom_header->signature));
		return NULL;
	}
	
	/* checksum */
	rom_bytes = (unsigned char *)rom_address;
	for (i = 0; i < rom_header->size * 512; i++)
	    sum += *(rom_bytes + i);

	if (sum != 0)
		printk(BIOS_ALERT, "Incorrent Expansion ROM checksum (%02x != 0)\n", sum);

	rom_data = (struct pci_data *)((unsigned char *)rom_header +
				       le32_to_cpu(rom_header->data));

	printk(BIOS_SPEW, "PCI ROM Image, Vendor %04x, Device %04x,\n",
	       rom_data->vendor, rom_data->device);
	if (dev->id.u.pci.vendor != rom_data->vendor || dev->id.u.pci.device != rom_data->device) {
		printk(BIOS_ERR,
		       "Device or Vendor ID mismatch Vendor %04x, Device %04x\n",
		       rom_data->vendor, rom_data->device);
		return NULL;
	}

	printk(BIOS_SPEW,
	       "PCI ROM Image, Class Code %04x%02x, Code Type %02x\n",
	       rom_data->class_hi, rom_data->class_lo, rom_data->type);
	if (dev->class != ((rom_data->class_hi << 8) | rom_data->class_lo)) {
		printk(BIOS_DEBUG, "Class Code mismatch ROM %08x, dev %08x\n",
		       (rom_data->class_hi << 8) | rom_data->class_lo,
		       dev->class);
		// return NULL;
	}

	return rom_header;
}

static void *pci_ram_image_start = (void *)PCI_RAM_IMAGE_START;

#ifdef CONFIG_PCI_OPTION_ROM_RUN
extern int vga_inited;		// Defined in vga_console.c.
#ifndef CONFIG_MULTIPLE_VGA_INIT
extern struct device *vga_pri;	// The primary VGA device, defined in device.c.
#endif
#endif

struct rom_header *pci_rom_load(struct device *dev,
				struct rom_header *rom_header)
{
	struct pci_data *rom_data;
	unsigned long rom_address;
	unsigned int rom_size;
	unsigned int image_size = 0;

	rom_address = pci_read_config32(dev, PCI_ROM_ADDRESS);

	do {
		rom_header = (struct rom_header *)((unsigned char *)rom_header + image_size);	// Get next image.
		rom_data = (struct pci_data *)((unsigned char *)rom_header +
					       le32_to_cpu(rom_header->data));
		image_size = le32_to_cpu(rom_data->ilen) * 512;
	} while ((rom_data->type != 0) && (rom_data->indicator != 0));	// Make sure we got x86 version.

	if (rom_data->type != 0)
		return NULL;

	rom_size = rom_header->size * 512;

	if (PCI_CLASS_DISPLAY_VGA == rom_data->class_hi) {
#ifdef CONFIG_PCI_OPTION_ROM_RUN
#ifndef CONFIG_MULTIPLE_VGA_INIT
		if (dev != vga_pri)
			return NULL;	// Only one VGA supported.
#endif
		if (rom_header != (void *)PCI_VGA_RAM_IMAGE_START) {
			printk(BIOS_DEBUG, "Copying VGA ROM image from %p to 0x%x, 0x%x bytes\n", 
					rom_header, PCI_VGA_RAM_IMAGE_START, rom_size);
			memcpy((void *)PCI_VGA_RAM_IMAGE_START, rom_header, rom_size);
		}
		vga_inited = 1;
		return (struct rom_header *)(PCI_VGA_RAM_IMAGE_START);
#endif
	} else {
		printk(BIOS_DEBUG,
		       "Copying non-VGA ROM image from %p to %p, 0x%x bytes\n",
		       rom_header, pci_ram_image_start, rom_size);
		memcpy(pci_ram_image_start, rom_header, rom_size);
		pci_ram_image_start += rom_size;
		return (struct rom_header *)(pci_ram_image_start - rom_size);
	}
	/* Disable expansion ROM address decoding. */
	pci_write_config32(dev, PCI_ROM_ADDRESS,
			   rom_address & ~PCI_ROM_ADDRESS_ENABLE);

	return NULL;
}
