/*
 * ahci.c: dump AHCI registers
 *
 * Copyright (C) 2016 Iru Cai
 * Copyright (C) 2017 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "inteltool.h"

static const char *ghc_regs[] = {
	"CAP", "GHC", "IS", "PI",
	"VS", "CCC_CTL", "CCC_PORTS", "EM_LOC",
	"EM_CTL", "CAP2", "BOHC"
};

static const char *port_ctl_regs[] = {
	"PxCLB", "PxCLBU", "PxFB", "PxFBU",
	"PxIS", "PxIE", "PxCMD", "Reserved",
	"PxTFD", "PxSIG", "PxSSTS", "PxSCTL",
	"PxSERR", "PxSACT", "PxCI", "PxSNTF",
	"PxFBS", "PxDEVSLP", "Reserved"
};

#define NUM_GHC (sizeof(ghc_regs)/sizeof(ghc_regs[0]))
#define NUM_PORTCTL (sizeof(port_ctl_regs)/sizeof(port_ctl_regs[0]))

int print_ahci(struct pci_dev *ahci)
{
	size_t mmio_size;
	uint8_t *mmio;
	uint32_t i, j;

	if (!ahci) {
		puts("No SATA device found");
		return 0;
	}
	printf("\n============= AHCI Registers ==============\n\n");

	if (ahci->device_id == PCI_DEVICE_ID_INTEL_SUNRISEPOINT_SATA)
		mmio_size = 0x800;
	else
		mmio_size = 0x400;

	const pciaddr_t mmio_phys = ahci->base_addr[5] & ~0x7ULL;
	printf("ABAR = 0x%08llx (MEM)\n\n", (unsigned long long)mmio_phys);
	mmio = map_physical(mmio_phys, mmio_size);
	if (mmio == NULL) {
		perror("Error mapping MMIO");
		exit(1);
	}

	puts("Generic Host Control Registers:");
	for (i = 0; i < 0x100; i += 4) {
		printf("0x%02x: 0x%08x (%s)\n", i, *(uint32_t *)(mmio + i),
				 (i / 4 < NUM_GHC) ? ghc_regs[i / 4]:"Reserved");
	}

	const size_t max_ports = (mmio_size - 0x100) / 0x80;
	for (i = 0; i < max_ports; i++) {
		if (*(uint32_t *)(mmio + 0x0c) & 1 << i) {
			printf("\nPort %d Control Registers:\n", i);
			uint8_t *mmio_port = mmio + 0x100 + i * 0x80;
			for (j = 0; j < 0x80; j += 4) {
				printf("0x%03x: 0x%08x (%s)\n", 0x100+i*0x80+j,
				       *(uint32_t *)(mmio_port + j),
				       (j / 4 < NUM_PORTCTL) ?
				       port_ctl_regs[j / 4] :
				       "Reserved");
			}
		}
	}

	if (ahci->device_id == PCI_DEVICE_ID_INTEL_SUNRISEPOINT_SATA) {
		puts("\nOther registers:");
		for (i = 0x500; i < mmio_size; i += 4)
			printf("0x%03x: 0x%08x\n", i, *(uint32_t *)(mmio + i));
	}

	unmap_physical((void *)mmio, mmio_size);
	return 0;
}
