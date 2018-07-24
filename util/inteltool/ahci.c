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

#define MMIO(offset) (*(uint32_t *)(mmio + offset))
#define MMIO_PORT(offset) (*(uint32_t *)(mmio_port + offset))

static void print_port(const uint8_t *const mmio, size_t port)
{
	size_t i;
	printf("\nPort %zu Control Registers:\n", port);
	const uint8_t *const mmio_port = mmio + 0x100 + port * 0x80;
	for (i = 0; i < 0x80; i += 4) {
		if (i / 4 < NUM_PORTCTL) {
			printf("0x%03zx: 0x%08x (%s)\n",
			       (size_t)(mmio_port - mmio) + i,
			       MMIO_PORT(i), port_ctl_regs[i / 4]);
		} else if (MMIO_PORT(i)) {
			printf("0x%03zx: 0x%08x (Reserved)\n",
			       (size_t)(mmio_port - mmio) + i, MMIO_PORT(i));
		}
	}
}

int print_ahci(struct pci_dev *ahci)
{
	size_t mmio_size, i;

	if (!ahci) {
		puts("No SATA device found");
		return 0;
	}
	printf("\n============= AHCI Registers ==============\n\n");

	if (ahci->device_id == PCI_DEVICE_ID_INTEL_SUNRISEPOINT_SATA ||
	    ahci->device_id == PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_SATA)
		mmio_size = 0x800;
	else
		mmio_size = 0x400;

	const pciaddr_t mmio_phys = ahci->base_addr[5] & ~0x7ULL;
	printf("ABAR = 0x%08llx (MEM)\n\n", (unsigned long long)mmio_phys);
	const uint8_t *const mmio = map_physical(mmio_phys, mmio_size);
	if (mmio == NULL) {
		perror("Error mapping MMIO");
		exit(1);
	}

	puts("Generic Host Control Registers:");
	for (i = 0; i < 0x100; i += 4) {
		if (i / 4 < NUM_GHC) {
			printf("0x%03zx: 0x%08x (%s)\n",
			       i, MMIO(i), ghc_regs[i / 4]);
		} else if (MMIO(i)) {
			printf("0x%03zx: 0x%08x (Reserved)\n", i, MMIO(i));
		}
	}

	const size_t max_ports = (mmio_size - 0x100) / 0x80;
	for (i = 0; i < max_ports; i++) {
		if (MMIO(0x0c) & 1 << i)
			print_port(mmio, i);
	}

	if (ahci->device_id == PCI_DEVICE_ID_INTEL_SUNRISEPOINT_SATA ||
	    ahci->device_id == PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_SATA) {
		puts("\nOther registers:");
		for (i = 0x500; i < mmio_size; i += 4) {
			if (MMIO(i))
				printf("0x%03zx: 0x%08x\n", i, MMIO(i));
		}
	}

	unmap_physical((void *)mmio, mmio_size);
	return 0;
}
