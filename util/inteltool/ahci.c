/* ahci.c: dump AHCI registers */
/* SPDX-License-Identifier: GPL-2.0-only */

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

static const io_register_t sunrise_ahci_cfg_registers[] = {
	{0x0, 4, "ID"},
	{0x4, 2, "CMD"},
	{0x6, 2, "STS"},
	{0x8, 1, "RID"},
	{0x9, 1, "PI"},
	{0xa, 2, "CC"},
	{0xc, 1, "CLS"},
	{0xd, 1, "MLT"},
	{0xe, 1, "HTYPE"},
	{0x10, 4, "MXTBA"},
	{0x14, 4, "MXPBA"},
	{0x20, 4, "AIDPBA"},
	{0x24, 4, "ABAR"},
	{0x2c, 4, "SS"},
	{0x34, 1, "CAP"},
	{0x3c, 2, "INTR"},
	{0x70, 2, "PID"},
	{0x72, 2, "PC"},
	{0x74, 2, "PMCS"},
	{0x80, 2, "MID"},
	{0x82, 2, "MC"},
	{0x84, 4, "MA"},
	{0x88, 2, "MD"},
	{0x90, 4, "MAP"},
	{0x94, 4, "PCS"},
	{0x9c, 4, "SATAGC"},
	{0xa0, 1, "SIRI"},
	{0xa4, 4, "SIRD"},
	{0xa8, 4, "SATACR0"},
	{0xac, 4, "SATACR1"},
	{0xc0, 4, "SP"},
	{0xd0, 2, "MXID"},
	{0xd2, 2, "MXC"},
	{0xd4, 4, "MXT"},
	{0xd8, 4, "MXP"},
	{0xe0, 4, "BFCS"},
	{0xe4, 4, "BFTD1"},
	{0xe8, 4, "BFTD2"},
};

static const io_register_t sunrise_ahci_sir_registers[] = {
	{0x80, 4, "SQUELCH"},
	{0x90, 4, "SATA_MPHY_PG"},
	{0xa4, 4, "OOBRETR"},
};

#define NUM_GHC (sizeof(ghc_regs)/sizeof(ghc_regs[0]))
#define NUM_PORTCTL (sizeof(port_ctl_regs)/sizeof(port_ctl_regs[0]))

static void print_port(const uint8_t *const mmio, size_t port)
{
	size_t i;
	printf("\nPort %zu Control Registers:\n", port);
	const uint8_t *const mmio_port = mmio + 0x100 + port * 0x80;
	for (i = 0; i < 0x80; i += 4) {
		if (i / 4 < NUM_PORTCTL) {
			printf("0x%03zx: 0x%08x (%s)\n",
			       (size_t)(mmio_port - mmio) + i,
			       read32(mmio_port + i), port_ctl_regs[i / 4]);
		} else if (read32(mmio_port + i)) {
			printf("0x%03zx: 0x%08x (Reserved)\n",
			       (size_t)(mmio_port - mmio) + i,
			       read32(mmio_port + i));
		}
	}
}

int print_ahci(struct pci_dev *ahci)
{
	size_t ahci_registers_size = 0, i;
	size_t ahci_cfg_registers_size = 0;
	const io_register_t *ahci_cfg_registers;
	size_t ahci_sir_offset = 0;
	size_t ahci_sir_registers_size = 0;
	const io_register_t *ahci_sir_registers;

	if (!ahci) {
		puts("No SATA device found");
		return 0;
	}
	printf("\n============= AHCI Registers ==============\n\n");

	switch (ahci->device_id) {
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_SATA:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_SATA:
		ahci_registers_size = 0x800;
		ahci_sir_offset = 0xa0;
		ahci_cfg_registers = sunrise_ahci_cfg_registers;
		ahci_cfg_registers_size = ARRAY_SIZE(sunrise_ahci_cfg_registers);
		ahci_sir_registers = sunrise_ahci_sir_registers;
		ahci_sir_registers_size = ARRAY_SIZE(sunrise_ahci_sir_registers);
		break;
	default:
		ahci_registers_size = 0x400;
	}

	printf("\n============= AHCI Configuration Registers ==============\n\n");
	for (i = 0; i < ahci_cfg_registers_size; i++) {
		switch (ahci_cfg_registers[i].size) {
		case 4:
			printf("0x%04x: 0x%08x (%s)\n",
				ahci_cfg_registers[i].addr,
				pci_read_long(ahci, ahci_cfg_registers[i].addr),
				ahci_cfg_registers[i].name);
			break;
		case 2:
			printf("0x%04x: 0x%04x     (%s)\n",
				ahci_cfg_registers[i].addr,
				pci_read_word(ahci, ahci_cfg_registers[i].addr),
				ahci_cfg_registers[i].name);
			break;
		case 1:
			printf("0x%04x: 0x%02x       (%s)\n",
				ahci_cfg_registers[i].addr,
				pci_read_byte(ahci, ahci_cfg_registers[i].addr),
				ahci_cfg_registers[i].name);
			break;
		}
	}

	printf("\n============= SATA Initialization Registers ==============\n\n");
	for (i = 0; i < ahci_sir_registers_size; i++) {
		pci_write_byte(ahci, ahci_sir_offset, ahci_sir_registers[i].addr);
		switch (ahci_sir_registers[i].size) {
		case 4:
			printf("0x%02x: 0x%08x (%s)\n",
				ahci_sir_registers[i].addr,
				pci_read_long(ahci, ahci_sir_offset),
				ahci_sir_registers[i].name);
			break;
		case 2:
			printf("0x%02x: 0x%04x     (%s)\n",
				ahci_sir_registers[i].addr,
				pci_read_word(ahci, ahci_sir_offset),
				ahci_sir_registers[i].name);
			break;
		case 1:
			printf("0x%02x: 0x%02x       (%s)\n",
				ahci_sir_registers[i].addr,
				pci_read_byte(ahci, ahci_sir_offset),
				ahci_sir_registers[i].name);
			break;
		}
	}

	const pciaddr_t ahci_phys = ahci->base_addr[5] & ~0x7ULL;
	printf("\n============= ABAR ==============\n\n");
	printf("ABAR = 0x%08llx (MEM)\n\n", (unsigned long long)ahci_phys);
	const uint8_t *const mmio = map_physical(ahci_phys, ahci_registers_size);
	if (mmio == NULL) {
		perror("Error mapping MMIO");
		exit(1);
	}

	puts("Generic Host Control Registers:");
	for (i = 0; i < 0x100; i += 4) {
		if (i / 4 < NUM_GHC) {
			printf("0x%03zx: 0x%08x (%s)\n",
			       i, read32(mmio + i), ghc_regs[i / 4]);
		} else if (read32(mmio + i)) {
			printf("0x%03zx: 0x%08x (Reserved)\n", i,
			       read32(mmio + i));
		}
	}

	const size_t max_ports = (ahci_registers_size - 0x100) / 0x80;
	for (i = 0; i < max_ports; i++) {
		if (read32(mmio + 0x0c) & 1 << i)
			print_port(mmio, i);
	}

	puts("\nOther registers:");
	for (i = 0x500; i < ahci_registers_size; i += 4) {
		if (read32(mmio + i))
			printf("0x%03zx: 0x%08x\n", i, read32(mmio + i));
	}

	unmap_physical((void *)mmio, ahci_registers_size);
	return 0;
}
