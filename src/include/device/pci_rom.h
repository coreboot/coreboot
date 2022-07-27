/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef PCI_ROM_H
#define PCI_ROM_H

#include <endian.h>
#include <acpi/acpi.h>
#include <stdint.h>

#define PCI_ROM_HDR 0xAA55
#define PCI_DATA_HDR ((uint32_t) (('R' << 24) | ('I' << 16) | ('C' << 8) | 'P'))

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

void vga_oprom_preload(void);
struct rom_header *pci_rom_probe(const struct device *dev);
struct rom_header *pci_rom_load(struct device *dev,
	struct rom_header *rom_header);

unsigned long
pci_rom_write_acpi_tables(const struct device *device,
						  unsigned long current,
						  struct acpi_rsdp *rsdp);

void pci_rom_ssdt(const struct device *device);

void map_oprom_vendev_rev(u32 *vendev, u8 *rev);
u32 map_oprom_vendev(u32 vendev);

int verified_boot_should_run_oprom(struct rom_header *rom_header);
#endif
