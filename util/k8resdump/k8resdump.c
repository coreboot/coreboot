/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
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

/* This program will dump the IO/memory/PCI resources from the K8
 * memory controller
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <pci/pci.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static uint8_t dram_bases[] =
    { 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78 };
static uint8_t dram_limits[] =
    { 0x44, 0x4C, 0x54, 0x5C, 0x64, 0x6C, 0x74, 0x7C };
static uint8_t iomem_bases[] =
    { 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8 };
static uint8_t iomem_limits[] =
    { 0x84, 0x8C, 0x94, 0x9C, 0xA4, 0xAC, 0xB4, 0xBC };

static uint8_t pciio_bases[] = { 0xC0, 0xC8, 0xD0, 0xD8 };
static uint8_t pciio_limits[] = { 0xC4, 0xCC, 0xD4, 0xDC };

void print_info(struct pci_dev *dev)
{
	int i;
	uint32_t regb, regl;

	for (i = 0; i < ARRAY_SIZE(dram_bases); i++) {
		regb = pci_read_long(dev, dram_bases[i]);
		regl = pci_read_long(dev, dram_limits[i]);

		printf
		    ("DRAM map: #%d 0x%04x000000 - 0x%04xffffff Access: %s/%s"
		     "  IntlvEN:0x%x IntlvSEL:0x%x Dstnode:%d\n",
		     i, regb >> 16, regl >> 16, regb & 1 ? "R" : "",
		     regb & 2 ? "W" : "", (regb & 0x700) >> 8,
		     (regl & 0x700) >> 8, (regl & 0x7));
	}


	for (i = 0; i < ARRAY_SIZE(iomem_bases); i++) {
		regb = pci_read_long(dev, iomem_bases[i]);
		regl = pci_read_long(dev, iomem_limits[i]);

		printf
		    ("MMIO map: #%d 0x%06x0000 - 0x%06xffff Access: %s/%s  %s %s"
		     " %s Dstnode:%d DstLink %d\n",
		     i, regb >> 8, regl >> 8, regb & 1 ? "R" : "",
		     regb & 2 ? "W" : "", regb & 4 ? "CPU Dis" : "",
		     regb & 8 ? "Locked" : "",
		     regl & 0x80 ? "NonPosted" : "", regl & 0x7,
		     (regl & 0x30) >> 4);
	}

	for (i = 0; i < ARRAY_SIZE(pciio_bases); i++) {
		regb = pci_read_long(dev, pciio_bases[i]);
		regl = pci_read_long(dev, pciio_limits[i]);

		printf
		    ("  IO map: #%d  0x%03x000 - 0x%03xfff Access: %s/%s  %s %s"
		     " Dstnode:%d DstLink %d\n",
		     i, (regb & ~0xff000000) >> 12,
		     (regl & ~0xff000000) >> 12, regb & 1 ? "R" : "",
		     regb & 2 ? "W" : "", regb & 0x20 ? "ISA" : "",
		     regb & 0x10 ? "VGA" : "", regl & 0x7,
		     (regl & 0x30) >> 4);
	}


}

int main(void)
{
	struct pci_access *pacc;
	struct pci_dev *dev;

	if (getuid()) {
		fprintf(stderr, "Please run me root, need access to all"
			" PCI regs!\n");
		exit(1);
	}

	pacc = pci_alloc();
	pci_init(pacc);
	pci_scan_bus(pacc);
	for (dev = pacc->devices; dev; dev = dev->next) {
		pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES |
			      PCI_FILL_CLASS);
		if ((dev->vendor_id == 0x1022) /* AMD */
		    && (dev->device_id == 0x1101)) { /* Address MAP */
			print_info(dev);
		}
	}
	pci_cleanup(pacc);
	return 0;
}
