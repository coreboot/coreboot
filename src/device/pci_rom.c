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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <string.h>
#include <cbfs.h>

struct rom_header *pci_rom_probe(struct device *dev)
{
	struct rom_header *rom_header;
	struct pci_data *rom_data;

	/* If it's in FLASH, then don't check device for ROM. */
	rom_header = cbfs_load_optionrom(CBFS_DEFAULT_MEDIA, dev->vendor,
					 dev->device, NULL);

	u32 vendev = (dev->vendor << 16) | dev->device;
	u32 mapped_vendev = vendev;

	if (map_oprom_vendev)
		mapped_vendev = map_oprom_vendev(vendev);

	if (!rom_header) {
		if (vendev != mapped_vendev) {
			rom_header = cbfs_load_optionrom(
					CBFS_DEFAULT_MEDIA,
					mapped_vendev >> 16,
					mapped_vendev & 0xffff, NULL);
		}
	}

	if (rom_header) {
		printk(BIOS_DEBUG, "In CBFS, ROM address for %s = %p\n",
		       dev_path(dev), rom_header);
	} else {
		u32 rom_address;

		rom_address = pci_read_config32(dev, PCI_ROM_ADDRESS);

		if (rom_address == 0x00000000 || rom_address == 0xffffffff) {
#if CONFIG_BOARD_EMULATION_QEMU_X86
			if ((dev->class >> 8) == PCI_CLASS_DISPLAY_VGA)
				rom_address = 0xc0000;
			else
#endif
				return NULL;
		} else {
			/* Enable expansion ROM address decoding. */
			pci_write_config32(dev, PCI_ROM_ADDRESS,
					   rom_address|PCI_ROM_ADDRESS_ENABLE);
		}

#if CONFIG_ON_DEVICE_ROM_RUN
		printk(BIOS_DEBUG, "Option ROM address for %s = %lx\n",
		       dev_path(dev), (unsigned long)rom_address);
		rom_header = (struct rom_header *)rom_address;
#else
		printk(BIOS_DEBUG, "Option ROM execution disabled "
			"for %s\n", dev_path(dev));
		return NULL;
#endif
	}

	printk(BIOS_SPEW, "PCI expansion ROM, signature 0x%04x, "
	       "INIT size 0x%04x, data ptr 0x%04x\n",
	       le32_to_cpu(rom_header->signature),
	       rom_header->size * 512, le32_to_cpu(rom_header->data));

	if (le32_to_cpu(rom_header->signature) != PCI_ROM_HDR) {
		printk(BIOS_ERR, "Incorrect expansion ROM header "
		       "signature %04x\n", le32_to_cpu(rom_header->signature));
		return NULL;
	}

	rom_data = (((void *)rom_header) + le32_to_cpu(rom_header->data));

	printk(BIOS_SPEW, "PCI ROM image, vendor ID %04x, device ID %04x,\n",
	       rom_data->vendor, rom_data->device);
	/* If the device id is mapped, a mismatch is expected */
	if ((dev->vendor != rom_data->vendor
	    || dev->device != rom_data->device)
	    && (vendev == mapped_vendev)) {
		printk(BIOS_ERR, "ID mismatch: vendor ID %04x, "
		       "device ID %04x\n", rom_data->vendor, rom_data->device);
		return NULL;
	}

	printk(BIOS_SPEW, "PCI ROM image, Class Code %04x%02x, "
	       "Code Type %02x\n", rom_data->class_hi, rom_data->class_lo,
	       rom_data->type);

	if (dev->class != ((rom_data->class_hi << 8) | rom_data->class_lo)) {
		printk(BIOS_DEBUG, "Class Code mismatch ROM %08x, dev %08x\n",
		       (rom_data->class_hi << 8) | rom_data->class_lo,
		       dev->class);
		// return NULL;
	}

	return rom_header;
}

static void *pci_ram_image_start = (void *)PCI_RAM_IMAGE_START;

struct rom_header *pci_rom_load(struct device *dev,
				struct rom_header *rom_header)
{
	struct pci_data * rom_data;
	unsigned int rom_size;
	unsigned int image_size=0;

	do {
		/* Get next image. */
		rom_header = (struct rom_header *)((void *) rom_header
							    + image_size);

		rom_data = (struct pci_data *)((void *) rom_header
				+ le32_to_cpu(rom_header->data));

		image_size = le32_to_cpu(rom_data->ilen) * 512;
	} while ((rom_data->type != 0) && (rom_data->indicator != 0)); // make sure we got x86 version

	if (rom_data->type != 0)
		return NULL;

	rom_size = rom_header->size * 512;

	/*
	 * We check to see if the device thinks it is a VGA device not
	 * whether the ROM image is for a VGA device because some
	 * devices have a mismatch between the hardware and the ROM.
	 */
 	if (PCI_CLASS_DISPLAY_VGA == (dev->class >> 8)) {
#if !CONFIG_MULTIPLE_VGA_ADAPTERS
		extern device_t vga_pri; /* Primary VGA device (device.c). */
		if (dev != vga_pri) return NULL; /* Only one VGA supported. */
#endif
		if ((void *)PCI_VGA_RAM_IMAGE_START != rom_header) {
			printk(BIOS_DEBUG, "Copying VGA ROM Image from %p to "
			       "0x%x, 0x%x bytes\n", rom_header,
			       PCI_VGA_RAM_IMAGE_START, rom_size);
			memcpy((void *)PCI_VGA_RAM_IMAGE_START, rom_header,
			       rom_size);
		}
		return (struct rom_header *) (PCI_VGA_RAM_IMAGE_START);
	}

	printk(BIOS_DEBUG, "Copying non-VGA ROM image from %p to %p, 0x%x "
	       "bytes\n", rom_header, pci_ram_image_start, rom_size);

	memcpy(pci_ram_image_start, rom_header, rom_size);
	pci_ram_image_start += rom_size;
	return (struct rom_header *) (pci_ram_image_start-rom_size);
}
