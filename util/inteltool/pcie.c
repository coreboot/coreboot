/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008 by coresystems GmbH 
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "inteltool.h"

/*
 * Egress Port Root Complex MMIO configuration space
 */
int print_epbar(struct pci_dev *nb)
{
	int i, size = (4 * 1024);
	volatile uint8_t *epbar;
	uint32_t epbar_phys;

	printf("\n============= EPBAR =============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82945GM:
		epbar_phys = pci_read_long(nb, 0x40) & 0xfffffffe;
		break;
	case 0x1234: // Dummy for non-existent functionality
		printf("This northbrigde does not have EPBAR.\n");
		return 1;
	default:
		printf("Error: Dumping EPBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	epbar = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
		     fd_mem, (off_t) epbar_phys);
	
	if (epbar == MAP_FAILED) {
		perror("Error mapping EPBAR");
		exit(1);
	}

	printf("EPBAR = 0x%08x (MEM)\n\n", epbar_phys);
	for (i = 0; i < size; i += 4) {
		if (*(uint32_t *)(epbar + i))
			printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(epbar+i));
	}

	munmap((void *)epbar, size);
	return 0;
}

/*
 * MCH-ICH Serial Interconnect Ingress Root Complex MMIO configuration space
 */
int print_dmibar(struct pci_dev *nb)
{
	int i, size = (4 * 1024);
	volatile uint8_t *dmibar;
	uint32_t dmibar_phys;

	printf("\n============= DMIBAR ============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82945GM:
		dmibar_phys = pci_read_long(nb, 0x4c) & 0xfffffffe;
		break;
	case 0x1234: // Dummy for non-existent functionality
		printf("This northbrigde does not have DMIBAR.\n");
		return 1;
	default:
		printf("Error: Dumping DMIBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	dmibar = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
		      fd_mem, (off_t) dmibar_phys);
	
	if (dmibar == MAP_FAILED) {
		perror("Error mapping DMIBAR");
		exit(1);
	}

	printf("DMIBAR = 0x%08x (MEM)\n\n", dmibar_phys);
	for (i = 0; i < size; i += 4) {
		if (*(uint32_t *)(dmibar + i))
			printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(dmibar+i));
	}

	munmap((void *)dmibar, size);
	return 0;
}

/*
 * PCIe MMIO configuration space
 */
int print_pciexbar(struct pci_dev *nb)
{
	uint32_t pciexbar_reg;
	uint32_t pciexbar_phys;
	volatile uint8_t *pciexbar;
	int max_busses, devbase, i;
	int bus, dev, fn;

	printf("========= PCIEXBAR ========\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82945GM:
		pciexbar_reg = pci_read_long(nb, 0x48);
		break;
	case 0x1234: // Dummy for non-existent functionality
		printf("Error: This northbrigde does not have PCIEXBAR.\n");
		return 1;
	default:
		printf("Error: Dumping PCIEXBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	if (!(pciexbar_reg & (1 << 0))) {
		printf("PCIEXBAR register is disabled.\n");
		return 0;
	}

	switch ((pciexbar_reg >> 1) & 3) {
	case 0: // 256MB
		pciexbar_phys = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28));
		max_busses = 256;
		break;
	case 1: // 128M
		pciexbar_phys = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28)|(1 << 27));
		max_busses = 128;
		break;
	case 2: // 64M
		pciexbar_phys = pciexbar_reg & ((1 << 31)|(1 << 30)|(1 << 29)|(1 << 28)|(1 << 27)|(1 << 26));
		max_busses = 64;
		break;
	default: // RSVD
		printf("Undefined address base. Bailing out.\n");
		return 1;
	}	

	printf("PCIEXBAR: 0x%08x\n", pciexbar_phys);

	pciexbar = mmap(0, (max_busses * 1024 * 1024), PROT_WRITE | PROT_READ,
		        MAP_SHARED, fd_mem, (off_t) pciexbar_phys);
	
	if (pciexbar == MAP_FAILED) {
		perror("Error mapping PCIEXBAR");
		exit(1);
	}
	
	for (bus = 0; bus < max_busses; bus++) {
		for (dev = 0; dev < 32; dev++) {
			for (fn = 0; fn < 8; fn++) {
				devbase = (bus * 1024 * 1024) + (dev * 32 * 1024) + (fn * 4 * 1024);

				if (*(uint16_t *)(pciexbar + devbase) == 0xffff)
					continue;
				
				/* This is a heuristics. Anyone got a better check? */
				if( (*(uint32_t *)(pciexbar + devbase + 256) == 0xffffffff) &&
					(*(uint32_t *)(pciexbar + devbase + 512) == 0xffffffff) ) {
#if DEBUG
					printf("Skipped non-PCIe device %02x:%02x.%01x\n", bus, dev, fn);
#endif
					continue;
				}

				printf("\nPCIe %02x:%02x.%01x extended config space:", bus, dev, fn);
				for (i = 0; i < 4096; i++) {
					if((i % 0x10) == 0)
						printf("\n%04x:", i);
					printf(" %02x", *(pciexbar+devbase+i));
				}
				printf("\n");
			}
		}
	}

	munmap((void *)pciexbar, (max_busses * 1024 * 1024));

	return 0;
}


