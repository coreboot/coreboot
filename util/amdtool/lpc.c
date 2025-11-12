/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <commonlib/helpers.h>
#include "amdtool.h"

static const io_register_t lpc_cfg_registers[] = {
	{0x00, 4, "ID"},
	{0x04, 2, "CMD"},
	{0x06, 2, "STS"},
	{0x08, 1, "RID"},
	{0x09, 1, "CC[2]"},
	{0x0A, 1, "CC[1]"},
	{0x0B, 1, "CC[0]"},
	{0x0C, 1, "CLSIZE"},
	{0x0D, 1, "LATTMR"},
	{0x0E, 1, "HTYPE"},
	{0x2C, 4, "SS"},
	{0x34, 1, "CAPP"},
	{0x40, 1, "PCICTL"},
	{0x44, 4, "IOPDE"},
	{0x48, 4, "IOMPDE"},
	{0x4C, 4, "MEMRNG"},
	{0x50, 4, "ROMPROT[0]"},
	{0x54, 4, "ROMPROT[1]"},
	{0x58, 4, "ROMPROT[2]"},
	{0x5C, 4, "ROMPROT[3]"},
	{0x60, 2, "LPCMEMADDRSTART"},
	{0x62, 2, "LPCMEMADDREND"},
	{0x64, 2, "PCIWIDEIO[0]"},
	{0x66, 2, "PCIWIDEIO[1]"},
	{0x68, 2, "ROMADDRSTART[0]"},
	{0x6A, 2, "ROMADDREND[0]"},
	{0x6C, 2, "ROMADDRSTART[1]"},
	{0x6E, 2, "ROMADDREND[1]"},
	{0x70, 4, "FWHUBSEL"},
	{0x74, 4, "ALTWIDEIO"},
	{0x78, 4, "MISCCTL"},
	{0x7C, 1, "TPM"},
	{0x7D, 1, "LPCCLKCNTL"},
	{0x84, 4, "TMKBCBASELO"},
	{0x88, 4, "TMKBCBASEHI"},
	{0x8C, 2, "TMKBCREMAP"},
	{0x90, 2, "PCIWIDEIO[2]"},
	{0x98, 4, "EC_LPC_CTL"},
	{0x9C, 4, "GEC_SHDWROM_ADDR"},
	{0xA0, 4, "SPIBASE"},
	{0xA4, 2, "EC_PORT_ADDR"},
	{0xA8, 4, "ROM3_LO"},
	{0xAC, 4, "ROM3_HI"},
	{0xB8, 4, "ROMDMACTL"},
	{0xBA, 1, "EC_CTL"},
	{0xBB, 1, "HOST_CTL"},
	{0xC8, 4, "CLIENT_ROM_PROTECT"},
	{0xCC, 4, "AUTO_ROM_CFG"},
	{0xD0, 4, "CLKCTL"},
	{0xD4, 4, "CLKRUNOPT"},
	{0xDC, 4, "MISCCTL"},
	{0xE0, 4, "ROMPROT[4]"},
	{0xE4, 4, "ROMPROT[5]"},
	{0xE8, 4, "ROMPROT[6]"},
	{0xEC, 4, "ROMPROT[7]"},
};

int print_lpc(struct pci_dev *sb)
{
	size_t i, cfg_registers_size = 0;
	const io_register_t *cfg_registers;
	int smbus_rev = 0;

	printf("\n========== LPC =========\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_AMD_FCH_LPC_2:
		smbus_rev = find_smbus_dev_rev(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_2);
		if (smbus_rev == -1)
			return 1;

		switch (smbus_rev) {
		case 0x71:
			cfg_registers = lpc_cfg_registers;
			cfg_registers_size = ARRAY_SIZE(lpc_cfg_registers);
			break;
		default:
			printf("Error: Dumping LPC on this southbridge is not (yet) supported.\n");
			return 1;
		}

		break;
	default:
		printf("Error: Dumping LPC on this southbridge is not (yet) supported.\n");
		return 1;
	}

	for (i = 0; i < cfg_registers_size; i++) {
		switch (cfg_registers[i].size) {
		case 4:
			printf("0x%04x: 0x%08x (%s)\n",
				cfg_registers[i].addr,
				pci_read_long(sb, cfg_registers[i].addr),
				cfg_registers[i].name);
			break;
		case 2:
			printf("0x%04x: 0x%04x     (%s)\n",
				cfg_registers[i].addr,
				pci_read_word(sb, cfg_registers[i].addr),
				cfg_registers[i].name);
			break;
		case 1:
			printf("0x%04x: 0x%02x       (%s)\n",
				cfg_registers[i].addr,
				pci_read_byte(sb, cfg_registers[i].addr),
				cfg_registers[i].name);
			break;
		default:
			printf("Error: register size %d not implemented.\n",
				cfg_registers[i].size);
			break;
		}
	}

	return 0;
}
