/*
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

*/
#ifndef PCI_ROM_H
#define PCI_ROM_H
#include <arch/byteorder.h>
#include <stddef.h>

#define PCI_ROM_HDR 0xAA55
#define PCI_DATA_HDR (uint32_t) ( ('R' << 24) | ('I' << 16) | ('C' << 8) | 'P' )

#define PCI_RAM_IMAGE_START 0xD0000
#define PCI_VGA_RAM_IMAGE_START 0xC0000

struct rom_header {
	uint16_t	signature;
	uint8_t		size;
	uint8_t		init[3];
	uint8_t		reserved[0x12];
	uint16_t	data;
};

struct  pci_data {
	uint32_t	signature;
	uint16_t	vendor;
	uint16_t	device;
	uint16_t	reserved_1;
	uint16_t	dlen;
	uint8_t		drevision;
	uint8_t		class_lo;
	uint16_t	class_hi;
	uint16_t	ilen;
	uint16_t	irevision;
	uint8_t		type;
	uint8_t		indicator;
	uint16_t	reserved_2;
};

extern struct rom_header * pci_rom_probe(struct device *dev);
extern struct rom_header *pci_rom_load(struct device *dev, struct rom_header *rom_header);

extern void pci_dev_init(struct device *dev);

#endif
