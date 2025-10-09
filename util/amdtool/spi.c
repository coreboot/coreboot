/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include "amdtool.h"
#include "smn.h"

#define AMD_FCH_SPIBAR_OFFSET	0xa0

#define BRH_SPI_SMN_BASE	0x2DC4000

static const io_register_t spi100_speed_cfg_fields[] = {
	{ 0x0, 4, "TPM Speed" },
	{ 0x4, 4, "Alt Speed" },
	{ 0x8, 4, "Fast Read Speed" },
	{ 0xc, 4, "Normal Speed" },
};

static const io_register_t spi100_host_prefetch_cfg_fields[] = {
	{ 0x0, 7, "Host Prefetch Size" },
	{ 0x7, 1, "Host Prefetch on 64B Boudnary" },
	{ 0x8, 4, "Host Hit Range" },
	{ 0xc, 1, "Host Will Hit Enable" },
	{ 0xd, 1, "Host Hit Soon Enable" },
	{ 0xe, 1, "Host Burst Enable" },
	{ 0xf, 1, "Host Burst to 4DW Enable" },
};

static const io_register_t kunlun_spi_cntl0_fields[] = {
	{ 0x0, 8, "Spi OpCode (Reserved)" },
	{ 0x8, 1, "Disable Index Retry" },
	{ 0x9, 1, "Index Cacheline Stop" },
	{ 0xA, 2, "ROM Protection Compare Range" },
	{ 0x12, 1, "SPI Read Mode [0]" },
	{ 0x15, 1, "Illegal Access" },
	{ 0x16, 1, "SPI MAC Access ROM Enable" },
	{ 0x17, 1, "SPI Host Access ROM Enable" },
	{ 0x1c, 1, "SPI Clock Gate" },
	{ 0x1d, 1, "SPI Read Mode [1]" },
	{ 0x1e, 1, "SPI Read Mode [2]" },
	{ 0x1f, 1, "SPI Busy" },
};

static const io_register_t kunlun_alt_spi_cs_fields[] = {
	{ 0x0, 2, "Alt SPI CS Enable" },
	{ 0x3, 1, "SPI Protect Enable 0" },
	{ 0x5, 1, "SPI Protect Lock" },
	{ 0x6, 1, "Lock SPI CS" },
};

static const char * const kunlun_spi100_speed_values[] = {
	"66.66 MHz",
	"33.33 MHz",
	"22.22 MHz",
	"16.66 Mhz",
	"100 MHz",
	"800 KHz",
	"Defined in SPIx6C[5:0]",
	"Defined in SPIx6C[13:8]"
};

static const io_register_t kunlun_spi_bar_registers[] = {
	{ 0x00, 4, "SPI Control 0" },
	{ 0x04, 4, "SPI Restricted Command" },
	{ 0x08, 4, "SPI Restricted Command 2" },
	{ 0x0C, 4, "SPI Control 1" },
	{ 0x10, 4, "ESPI Control" },
	{ 0x14, 4, "SPI Command Value 1" },
	{ 0x18, 4, "SPI Command Value 2" },
	{ 0x1C, 1, "ROMCP CS" },
	{ 0x1D, 1, "SPI ALT CS" },
	{ 0x20, 4, "SPI100 Enable" },
	{ 0x24, 4, "SPI100 Precyc0" },
	{ 0x28, 4, "SPI100 Precyc1" },
	{ 0x2C, 2, "SPI100 Host Prefetch Config" },
	{ 0x2E, 2, "TPM SPI DI Timeout" },
	{ 0x30, 2, "ROM2 Address Override" },
	{ 0x32, 2, "SPI100 Dummy Cycle Config" },
	{ 0x32, 2, "SPI100 Rx Timing Config" },
	{ 0x40, 1, "DDR Command Code" },
	{ 0x41, 1, "QDR Command Code" },
	{ 0x42, 1, "DPR Command Code" },
	{ 0x43, 1, "QPR Command Code" },
	{ 0x44, 1, "Mode Byte" },
	{ 0x50, 4, "Addr32 Control 0" },
	{ 0x54, 4, "Addr32 Control 1" },
	{ 0x58, 4, "Addr32 Control 2" },
	{ 0x5C, 4, "Addr32 Control 3" },
	{ 0x60, 4, "BAR 64MB ROM3 Low" },
	{ 0x64, 4, "BAR 64MB ROM3 High" },
	{ 0x68, 4, "SPI Restricted Command 3" },
	{ 0x6C, 4, "SPI Speed" },
	{ 0x70, 4, "Host Interrupt" },
	{ 0x74, 4, "LPC SPI Interrupt" },
	{ 0x78, 4, "Flash Idle COunt" },
	{ 0xFC, 2, "SPI Misc Control" },
};


int use_smn_access = 0;
volatile uint8_t *spibar = NULL;
uint32_t spi_smn_base = 0;

static uint8_t spi_read8(uint32_t offset)
{
	if (use_smn_access)
		return smn_read8(spi_smn_base + offset);
	else
		return read8(spibar + offset);
}

static uint16_t spi_read16(uint32_t offset)
{
	if (use_smn_access)
		return smn_read16(spi_smn_base + offset);
	else
		return read16(spibar + offset);
}

static uint32_t spi_read32(uint32_t offset)
{
	if (use_smn_access)
		return smn_read32(spi_smn_base + offset);
	else
		return read32(spibar + offset);
}

static int print_spi_cntrl0(struct pci_dev *sb)
{
	int i, size = 0;
	int smbus_rev = 0;
	uint32_t spi_cntrl0 = UINT32_MAX;
	const io_register_t *spi_cntrl_register = NULL;

	printf("\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_AMD_FCH_LPC_2:
		smbus_rev = find_smbus_dev_rev(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_2);
		if (smbus_rev == -1)
			return 1;

		switch (smbus_rev) {
		case 0x71:
			spi_cntrl0 = spi_read32(0);
			spi_cntrl_register = kunlun_spi_cntl0_fields;
			size = ARRAY_SIZE(kunlun_spi_cntl0_fields);
			break;
		default:
			printf("Error: Dumping SPI CNTRL on this southbridge is not (yet) supported.\n");
			return 1;
		}

		break;
	default:
		printf("Error: Dumping SPI CNTRL on this southbridge is not (yet) supported.\n");
		return 1;
	}

	printf("SPI_CNTRL0 = 0x%08x\n\n", spi_cntrl0);

	if (spi_cntrl_register) {
		for (i = 0; i < size; i++) {
			unsigned int val = spi_cntrl0 >> spi_cntrl_register[i].addr;
			val &= ((1 << spi_cntrl_register[i].size) -1);
			printf("0x%04x = %s\n", val, spi_cntrl_register[i].name);
		}
	}

	return 0;
}

static int print_spi_alt_cs(struct pci_dev *sb)
{
	int i, size = 0;
	int smbus_rev = 0;
	uint8_t spi_alt_cs = 0xff;
	const io_register_t *spi_alt_cs_register = NULL;

	printf("\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_AMD_FCH_LPC_2:
		smbus_rev = find_smbus_dev_rev(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_2);
		if (smbus_rev == -1)
			return 1;

		switch (smbus_rev) {
		case 0x71:
			spi_alt_cs = spi_read8(0x1d);
			spi_alt_cs_register = kunlun_alt_spi_cs_fields;
			size = ARRAY_SIZE(kunlun_alt_spi_cs_fields);
			break;
		default:
			printf("Error: Dumping SPI_ALT_CS on this southbridge is not (yet) supported.\n");
			return 1;
		}

		break;
	default:
		printf("Error: Dumping SPI_ALT_CS on this southbridge is not (yet) supported.\n");
		return 1;
	}

	printf("SPI_ALT_CS = 0x%02x\n\n", spi_alt_cs);

	if (spi_alt_cs_register) {
		for (i = 0; i < size; i++) {
			unsigned int val = spi_alt_cs >> spi_alt_cs_register[i].addr;
			val &= ((1 << spi_alt_cs_register[i].size) -1);
			printf("0x%04x = %s\n", val, spi_alt_cs_register[i].name);
		}
	}

	return 0;
}

static int print_spi_speed_config(struct pci_dev *sb)
{
	size_t i, size = 0, values_size = 0;
	uint16_t spi_speed_cfg = UINT16_MAX;
	const io_register_t *spi_speed_cfg_register = NULL;
	const char * const *speed_values = NULL;
	int smbus_rev = 0;

	printf("\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_AMD_FCH_LPC_2:
		smbus_rev = find_smbus_dev_rev(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_2);
		if (smbus_rev == -1)
			return 1;

		switch (smbus_rev) {
		case 0x71:
			spi_speed_cfg = spi_read16(0x22);
			spi_speed_cfg_register = spi100_speed_cfg_fields;
			size = ARRAY_SIZE(spi100_speed_cfg_fields);
			speed_values = kunlun_spi100_speed_values;
			values_size = ARRAY_SIZE(kunlun_spi100_speed_values);
			break;
		default:
			printf("Error: Dumping SPI100_SPEED_CFG on this southbridge is not (yet) supported.\n");
			return 1;
		}

		break;
	default:
		printf("Error: Dumping SPI100_SPEED_CFG on this southbridge is not (yet) supported.\n");
		return 1;
	}

	printf("SPI100_SPEED_CFG = 0x%04x\n\n", spi_speed_cfg);

	if (spi_speed_cfg_register && speed_values) {
		for (i = 0; i < size; i++) {
			unsigned int val = spi_speed_cfg >> spi_speed_cfg_register[i].addr;
			val &= ((1 << spi_speed_cfg_register[i].size) -1);
			if (val < values_size)
				printf("0x%04x = %s (%s)\n", val, spi_speed_cfg_register[i].name,
							     speed_values[val]);
			else
				printf("0x%04x = %s (Reserved)\n", val, spi_speed_cfg_register[i].name);
		}
	}

	return 0;
}

static int print_spi_host_prefetch(struct pci_dev *sb)
{
	int i, size = 0;
	uint16_t spi_host_prefetch = UINT16_MAX;
	const io_register_t *spi_host_prefetch_register = NULL;
	int smbus_rev = 0;

	printf("\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_AMD_FCH_LPC_2:
		smbus_rev = find_smbus_dev_rev(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_2);
		if (smbus_rev == -1)
			return 1;

		switch (smbus_rev) {
		case 0x71:
			spi_host_prefetch = spi_read16(0x2c);
			spi_host_prefetch_register = spi100_host_prefetch_cfg_fields;
			size = ARRAY_SIZE(spi100_host_prefetch_cfg_fields);
			break;
		default:
			printf("Error: Dumping SPI100_HOST_PREFETCH_CFG on this southbridge is not (yet) supported.\n");
			return 1;
		}

		break;
	default:
		printf("Error: Dumping SPI100_HOST_PREFETCH_CFG on this southbridge is not (yet) supported.\n");
		return 1;
	}

	printf("SPI100_HOST_PREFETCH_CFG = 0x%04x\n\n", spi_host_prefetch);

	if (spi_host_prefetch_register) {
		for (i = 0; i < size; i++) {
			unsigned int val = spi_host_prefetch >> spi_host_prefetch_register[i].addr;
			val &= ((1 << spi_host_prefetch_register[i].size) - 1);
			printf("0x%04x = %s\n", val, spi_host_prefetch_register[i].name);
		}
	}

	return 0;
}

static int print_spibar(struct pci_dev *sb)
{
	int i, size = 0, spi_size = 0x1000;
	uint32_t spibar_phys, spibar_mask;
	const io_register_t *spi_register = NULL;
	int smbus_rev;

	printf("\n============= SPI Bar ==============\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_AMD_FCH_LPC_2:
		smbus_rev = find_smbus_dev_rev(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_2);
		if (smbus_rev == -1)
			return 1;

		switch (smbus_rev) {
		case 0x71:
			size = ARRAY_SIZE(kunlun_spi_bar_registers);
			spi_register = kunlun_spi_bar_registers;
			spibar_mask = 0xffffff00;
			spi_smn_base = BRH_SPI_SMN_BASE;
			break;
		default:
			printf("Error: Dumping SPI on this southbridge is not (yet) supported.\n");
			return 1;
		}

		break;
	default:
		printf("Error: Dumping SPI on this southbridge is not (yet) supported.\n");
		return 1;
	}

	spibar_phys = pci_read_long(sb, AMD_FCH_SPIBAR_OFFSET);
	if ((spibar_phys & spibar_mask) == 0 || (spibar_phys == 0xffffffff)) {
		if (spi_smn_base != 0) {
			use_smn_access = 1;
		} else {
			perror("Error SPIBAR not programmed");
			return 1;
		}
	} else {
		if (spibar_phys == UINT32_MAX) {
			perror("Error SPIBAR invalid");
			return 1;
		}

		spibar = map_physical(spibar_phys & spibar_mask, spi_size);
		if (spibar == NULL) {
			perror("Error mapping SPIBAR");
			return 1;
		}

	}

	for (i = 0; i < size; i++) {
		switch(spi_register[i].size) {
			case 1:
				printf("0x%08x = %s\n", spi_read8(spi_register[i].addr), spi_register[i].name);
				break;
			case 2:
				printf("0x%08x = %s\n", spi_read16(spi_register[i].addr), spi_register[i].name);
				break;
			case 4:
				printf("0x%08x = %s\n", spi_read32(spi_register[i].addr), spi_register[i].name);
				break;
			case 8:
				printf("0x%08x%08x = %s\n", spi_read32(spi_register[i].addr + 4),
					spi_read32(spi_register[i].addr), spi_register[i].name);
				break;
		}
	}

	print_spi_cntrl0(sb);
	print_spi_alt_cs(sb);
	print_spi_speed_config(sb);
	print_spi_host_prefetch(sb);

	if (spibar)
		unmap_physical((void *)spibar, spi_size);

	printf("\n====================================\n");

	return 0;
}

int print_spi(struct pci_dev *sb)
{
	return print_spibar(sb);
}
