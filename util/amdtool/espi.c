/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <commonlib/helpers.h>
#include "amdtool.h"
#include "smn.h"

#define AMD_FCH_SPIBAR_OFFSET	0xa0

#define SPIBAR_ESPI_MMIO_OFFSET	0x10000
#define SPI_MMIO_BASE		0xfec10000
#define ESPI0_MMIO_BASE		0xfec20000
#define ESPI1_MMIO_BASE		0xfec30000
#define ESPI_MMIO_SIZE		0x10000

#define ESPI0_SMN_BASE		0x02DC5000
#define ESPI1_SMN_BASE		0x02DCA000

static const io_register_t espi_cfg_registers[] = {
	{0x2C, 4, "MASTER_CAP"},
	{0x30, 4, "GLBL_CTL0"},
	{0x34, 4, "GLBL_CTL1"},
	{0x40, 4, "SLAVE0_DECODE_EN"},
	{0x44, 2, "IO_BASE[0]"},
	{0x46, 2, "IO_BASE[1]"},
	{0x48, 2, "IO_BASE[2]"},
	{0x4A, 2, "IO_BASE[3]"},
	{0x4C, 1, "IO_SIZE[0]"},
	{0x4D, 1, "IO_SIZE[1]"},
	{0x4E, 1, "IO_SIZE[2]"},
	{0x4F, 1, "IO_SIZE[3]"},
	{0x50, 4, "MMIO_BASE[0]"},
	{0x54, 4, "MMIO_BASE[1]"},
	{0x58, 4, "MMIO_BASE[2]"},
	{0x5C, 4, "MMIO_BASE[3]"},
	{0x60, 2, "MMIO_SIZE[0]"},
	{0x62, 2, "MMIO_SIZE[1]"},
	{0x64, 2, "MMIO_SIZE[2]"},
	{0x66, 2, "MMIO_SIZE[3]"},
	{0x68, 4, "SLAVE0_CFG"},
	{0x6C, 4, "SLAVE0_INT_EN"},
	{0x70, 4, "SLAVE0_INT_STS"},
	{0x80, 2, "IO_BASE[4]"},
	{0x82, 2, "IO_BASE[5]"},
	{0x84, 2, "IO_BASE[6]"},
	{0x86, 2, "IO_BASE[7]"},
	{0x88, 1, "IO_SIZE[4]"},
	{0x89, 1, "IO_SIZE[5]"},
	{0x8A, 1, "IO_SIZE[6]"},
	{0x8B, 1, "IO_SIZE[7]"},
	{0x8C, 2, "IO_BASE[8]"},
	{0x8E, 2, "IO_BASE[9]"},
	{0x90, 2, "IO_BASE[10]"},
	{0x92, 2, "IO_BASE[11]"},
	{0x94, 1, "IO_SIZE[8]"},
	{0x95, 1, "IO_SIZE[9]"},
	{0x96, 1, "IO_SIZE[10]"},
	{0x97, 1, "IO_SIZE[11]"},
	{0xA8, 4, "SLAVE0_RXVW_MISC_CNTL"},
	{0xAC, 4, "SLAVE0_RXVW_POLARITY"},
	{0xB0, 2, "IO_BASE[12]"},
	{0xB2, 2, "IO_BASE[13]"},
	{0xB4, 2, "IO_BASE[14]"},
	{0xB6, 2, "IO_BASE[15]"},
	{0xB8, 1, "IO_SIZE[12]"},
	{0xB9, 1, "IO_SIZE[13]"},
	{0xBA, 1, "IO_SIZE[14]"},
	{0xBB, 1, "IO_SIZE[15]"},
	{0xBC, 4, "MMIO_BASE[4]"},
	{0xC0, 4, "MMIO_SIZE[4]"},
	{0xC4, 4, "MMIO_CPU_TEMP"},
	{0xC8, 4, "MMIO_RTC_TIME"},
	{0xCC, 4, "ESPI_MISC_CTL1"},
};

static bool use_smn = false;
static uint32_t espi_smn_addr[2] = { 0, 0 };
static volatile uint8_t *espibar;

static uint32_t espi_read32(const io_register_t *reg, size_t espi_cntrlr)
{
	if (use_smn)
		return smn_read32(espi_smn_addr[espi_cntrlr] + reg->addr);
	else
		return read32(espibar + (espi_cntrlr * ESPI_MMIO_SIZE) + reg->addr);
}

static uint32_t espi_read16(const io_register_t *reg, size_t espi_cntrlr)
{
	if (use_smn)
		return smn_read16(espi_smn_addr[espi_cntrlr] + reg->addr);
	else
		return read16(espibar + (espi_cntrlr * ESPI_MMIO_SIZE) + reg->addr);
}

static uint32_t espi_read8(const io_register_t *reg, size_t espi_cntrlr)
{
	if (use_smn)
		return smn_read8(espi_smn_addr[espi_cntrlr] + reg->addr);
	else
		return read8(espibar + (espi_cntrlr * ESPI_MMIO_SIZE) + reg->addr);
}

int print_espi(struct pci_dev *sb, struct pci_dev *nb)
{
	size_t i, espi, cfg_registers_size = 0;
	uint32_t spibar_phys, spibar_mask;
	const io_register_t *cfg_registers;
	int smbus_rev = 0;
	size_t num_espi = 1;

	printf("\n========== eSPI ==========\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_AMD_FCH_LPC_2:
		smbus_rev = find_smbus_dev_rev(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_2);
		if (smbus_rev == -1)
			return 1;

		switch (smbus_rev) {
		case 0x71:
			if (nb->device_id == PCI_DEVICE_ID_AMD_BRH_ROOT_COMPLEX)
				num_espi = 2;

			cfg_registers = espi_cfg_registers;
			cfg_registers_size = ARRAY_SIZE(espi_cfg_registers);
			espi_smn_addr[0] = ESPI0_SMN_BASE;
			espi_smn_addr[1] = ESPI1_SMN_BASE;
			spibar_mask = 0xffffff00;
			break;
		default:
			printf("Error: Dumping eSPI on this southbridge is not (yet) supported.\n");
			return 1;
		}

		break;
	default:
		printf("Error: Dumping eSPI on this southbridge is not (yet) supported.\n");
		return 1;
	}

	spibar_phys = pci_read_long(sb, AMD_FCH_SPIBAR_OFFSET);
	if ((spibar_phys & spibar_mask) == 0) {
		perror("Error SPIBAR not programmed");
		return 1;
	}

	if (spibar_phys == UINT32_MAX) {
		spibar_phys = SPI_MMIO_BASE;
	}

	spibar_phys &= spibar_mask;

	espibar = map_physical(spibar_phys + SPIBAR_ESPI_MMIO_OFFSET,
				ESPI_MMIO_SIZE * num_espi);
	if (espibar == NULL) {
		perror("Error mapping ESPI BAR, trying SMN");
		use_smn = true;
	}

	for (espi = 0; espi < num_espi; espi++) {
		printf("\n---------- eSPI %lu ----------\n\n", espi);
		if (use_smn)
			printf("ESPI%lu 0x%08x (SMN)\n\n", espi, espi_smn_addr[espi]);
		else
			printf("ESPI%lu 0x%08x (MEM)\n\n", espi,
				(uint32_t)(spibar_phys + SPIBAR_ESPI_MMIO_OFFSET + (ESPI_MMIO_SIZE * espi)));

		for (i = 0; i < cfg_registers_size; i++) {
			switch (cfg_registers[i].size) {
			case 4:
				printf("0x%04x: 0x%08x (%s)\n",
					cfg_registers[i].addr,
					espi_read32(&cfg_registers[i], espi),
					cfg_registers[i].name);
				break;
			case 2:
				printf("0x%04x: 0x%04x     (%s)\n",
					cfg_registers[i].addr,
					espi_read16(&cfg_registers[i], espi),
					cfg_registers[i].name);
				break;
			case 1:
				printf("0x%04x: 0x%02x       (%s)\n",
					cfg_registers[i].addr,
					espi_read8(&cfg_registers[i], espi),
					cfg_registers[i].name);
				break;
			default:
				printf("Error: register size %d not implemented.\n",
					cfg_registers[i].size);
				break;
			}
		}
	}

	return 0;
}
