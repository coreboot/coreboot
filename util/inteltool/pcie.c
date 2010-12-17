/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008-2010 by coresystems GmbH
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
#include "inteltool.h"

/*
 * Egress Port Root Complex MMIO configuration space
 */
int print_epbar(struct pci_dev *nb)
{
	int i, size = (4 * 1024);
	volatile uint8_t *epbar;
	uint64_t epbar_phys;

	printf("\n============= EPBAR =============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82915:
	case PCI_DEVICE_ID_INTEL_82945GM:
	case PCI_DEVICE_ID_INTEL_82945GSE:
	case PCI_DEVICE_ID_INTEL_82945P:
	case PCI_DEVICE_ID_INTEL_82975X:
		epbar_phys = pci_read_long(nb, 0x40) & 0xfffffffe;
		break;
 	case PCI_DEVICE_ID_INTEL_PM965:
 	case PCI_DEVICE_ID_INTEL_Q965:
 	case PCI_DEVICE_ID_INTEL_82Q35:
 	case PCI_DEVICE_ID_INTEL_82G33:
 	case PCI_DEVICE_ID_INTEL_82Q33:
	case PCI_DEVICE_ID_INTEL_GS45:
	case PCI_DEVICE_ID_INTEL_ATOM_DXXX:
	case PCI_DEVICE_ID_INTEL_ATOM_NXXX:
 		epbar_phys = pci_read_long(nb, 0x40) & 0xfffffffe;
 		epbar_phys |= ((uint64_t)pci_read_long(nb, 0x44)) << 32;
 		break;
	case PCI_DEVICE_ID_INTEL_82810:
	case PCI_DEVICE_ID_INTEL_82810DC:
	case PCI_DEVICE_ID_INTEL_82810E_MC:
	case PCI_DEVICE_ID_INTEL_82830M:
	case PCI_DEVICE_ID_INTEL_82865:
		printf("This northbridge does not have EPBAR.\n");
		return 1;
	default:
		printf("Error: Dumping EPBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	epbar = map_physical(epbar_phys, size);

	if (epbar == NULL) {
		perror("Error mapping EPBAR");
		exit(1);
	}

	printf("EPBAR = 0x%08llx (MEM)\n\n", epbar_phys);
	for (i = 0; i < size; i += 4) {
		if (*(uint32_t *)(epbar + i))
			printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(epbar+i));
	}

	unmap_physical((void *)epbar, size);
	return 0;
}

/*
 * MCH-ICH Serial Interconnect Ingress Root Complex MMIO configuration space
 */
int print_dmibar(struct pci_dev *nb)
{
	int i, size = (4 * 1024);
	volatile uint8_t *dmibar;
	uint64_t dmibar_phys;

	printf("\n============= DMIBAR ============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82915:
	case PCI_DEVICE_ID_INTEL_82945GM:
	case PCI_DEVICE_ID_INTEL_82945GSE:
	case PCI_DEVICE_ID_INTEL_82945P:
	case PCI_DEVICE_ID_INTEL_82975X:
		dmibar_phys = pci_read_long(nb, 0x4c) & 0xfffffffe;
		break;
	case PCI_DEVICE_ID_INTEL_PM965:
	case PCI_DEVICE_ID_INTEL_Q965:
	case PCI_DEVICE_ID_INTEL_82Q35:
	case PCI_DEVICE_ID_INTEL_82G33:
	case PCI_DEVICE_ID_INTEL_82Q33:
	case PCI_DEVICE_ID_INTEL_GS45:
	case PCI_DEVICE_ID_INTEL_ATOM_DXXX:
	case PCI_DEVICE_ID_INTEL_ATOM_NXXX:
		dmibar_phys = pci_read_long(nb, 0x68) & 0xfffffffe;
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		break;
	case PCI_DEVICE_ID_INTEL_82810:
	case PCI_DEVICE_ID_INTEL_82810DC:
	case PCI_DEVICE_ID_INTEL_82810E_MC:
	case PCI_DEVICE_ID_INTEL_82865:
		printf("This northbridge does not have DMIBAR.\n");
		return 1;
	case PCI_DEVICE_ID_INTEL_X58:
		dmibar_phys = pci_read_long(nb, 0x50) & 0xfffff000;
		break;
	default:
		printf("Error: Dumping DMIBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	dmibar = map_physical(dmibar_phys, size);

	if (dmibar == NULL) {
		perror("Error mapping DMIBAR");
		exit(1);
	}

	printf("DMIBAR = 0x%08llx (MEM)\n\n", dmibar_phys);
	for (i = 0; i < size; i += 4) {
		if (*(uint32_t *)(dmibar + i))
			printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(dmibar+i));
	}

	unmap_physical((void *)dmibar, size);
	return 0;
}

/*
 * PCIe MMIO configuration space
 */
int print_pciexbar(struct pci_dev *nb)
{
	uint64_t pciexbar_reg;
	uint64_t pciexbar_phys;
	volatile uint8_t *pciexbar;
	int max_busses, devbase, i;
	int bus, dev, fn;

	printf("========= PCIEXBAR ========\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82915:
	case PCI_DEVICE_ID_INTEL_82945GM:
	case PCI_DEVICE_ID_INTEL_82945GSE:
	case PCI_DEVICE_ID_INTEL_82945P:
	case PCI_DEVICE_ID_INTEL_82975X:
		pciexbar_reg = pci_read_long(nb, 0x48);
		break;
 	case PCI_DEVICE_ID_INTEL_PM965:
	case PCI_DEVICE_ID_INTEL_Q965:
 	case PCI_DEVICE_ID_INTEL_82Q35:
 	case PCI_DEVICE_ID_INTEL_82G33:
 	case PCI_DEVICE_ID_INTEL_82Q33:
	case PCI_DEVICE_ID_INTEL_GS45:
	case PCI_DEVICE_ID_INTEL_ATOM_DXXX:
	case PCI_DEVICE_ID_INTEL_ATOM_NXXX:
 		pciexbar_reg = pci_read_long(nb, 0x60);
 		pciexbar_reg |= ((uint64_t)pci_read_long(nb, 0x64)) << 32;
 		break;
	case PCI_DEVICE_ID_INTEL_82810:
	case PCI_DEVICE_ID_INTEL_82810DC:
	case PCI_DEVICE_ID_INTEL_82810E_MC:
	case PCI_DEVICE_ID_INTEL_82865:
		printf("Error: This northbridge does not have PCIEXBAR.\n");
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
		pciexbar_phys = pciexbar_reg & (0xff << 28);
		max_busses = 256;
		break;
	case 1: // 128M
		pciexbar_phys = pciexbar_reg & (0x1ff << 27);
		max_busses = 128;
		break;
	case 2: // 64M
		pciexbar_phys = pciexbar_reg & (0x3ff << 26);
		max_busses = 64;
		break;
	default: // RSVD
		printf("Undefined address base. Bailing out.\n");
		return 1;
	}

	printf("PCIEXBAR: 0x%08llx\n", pciexbar_phys);

	pciexbar = map_physical(pciexbar_phys, (max_busses * 1024 * 1024));

	if (pciexbar == NULL) {
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

	unmap_physical((void *)pciexbar, (max_busses * 1024 * 1024));

	return 0;
}
