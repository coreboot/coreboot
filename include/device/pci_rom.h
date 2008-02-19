/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef DEVICE_PCI_ROM_H
#define DEVICE_PCI_ROM_H

#include <types.h>
#include <byteorder.h>
#include <device/device.h>

#define PCI_ROM_HDR 0xAA55
#define PCI_DATA_HDR (u32) ( ('R' << 24) | ('I' << 16) | ('C' << 8) | 'P' )

#define PCI_RAM_IMAGE_START 0xD0000
#define PCI_VGA_RAM_IMAGE_START 0xC0000

struct rom_header {
	u16	signature;
	u8	size;
	u8	init[3];
	u8	reserved[0x12];
	u16	data;
};

struct  pci_data {
	u32	signature;
	u16	vendor;
	u16	device;
	u16	reserved_1;
	u16	dlen;
	u8	drevision;
	u8	class_lo;
	u16	class_hi;
	u16	ilen;
	u16	irevision;
	u8	type;
	u8	indicator;
	u16	reserved_2;
};

extern struct rom_header * pci_rom_probe(struct device *dev);
extern struct rom_header *pci_rom_load(struct device *dev, struct rom_header *rom_header);

extern void pci_dev_init(struct device *dev);

#endif /* DEVICE_PCI_ROM_H */
