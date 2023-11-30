/* inteltool - dump all registers on an Intel CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <commonlib/helpers.h>
#include "inteltool.h"

#define SUNRISE_LPC_BC	0xdc

static const io_register_t sunrise_lpc_cfg_registers[] = {
	{0x00, 4, "ID"},
	{0x04, 2, "CMD"},
	{0x06, 2, "STS"},
	{0x08, 1, "RID"},
	{0x09, 1, "CC[3]"},
	{0x0A, 1, "CC[2]"},
	{0x0B, 1, "CC[1]"},
	{0x0C, 1, "CC[0]"},
	{0x0E, 1, "HTYPE"},
	{0x2C, 4, "SS"},
	{0x34, 1, "CAPP"},
	{0x64, 1, "SCNT"},
	{0x80, 2, "IOD"},
	{0x82, 2, "IOE"},
	{0x84, 4, "LGIR1"},
	{0x88, 4, "LGIR2"},
	{0x8C, 4, "LGIR3"},
	{0x90, 4, "LGIR4"},
	{0x94, 4, "ULKMC"},
	{0x98, 4, "LGMR"},
	{0xD0, 2, "FS1"},
	{0xD4, 2, "FS2"},
	{0xD8, 2, "BDE"},
	{0xDC, 1, "BC"},
	{0xE0, 4, "PCCTL"},
};

static const io_register_t sunrise_espi_cfg_registers[] = {
	{0x00, 4, "ESPI_DID_VID"},
	{0x04, 4, "ESPI_STS_CMD"},
	{0x08, 4, "ESPI_CC_RID"},
	{0x0C, 4, "ESPI_BIST_HTYPE_PLT_CLS"},
	{0x2C, 4, "ESPI_SS"},
	{0x34, 4, "ESPI_CAPP"},
	{0x80, 4, "ESPI_IOD_IOE"},
	{0x84, 4, "ESPI_LGIR1"},
	{0x88, 4, "ESPI_LGIR2"},
	{0x8C, 4, "ESPI_LGIR3"},
	{0x90, 4, "ESPI_LGIR4"},
	{0x94, 4, "ESPI_ULKMC"},
	{0x98, 4, "ESPI_LGMR"},
	{0xD0, 4, "ESPI_FS1"},
	{0xD4, 4, "ESPI_FS2"},
	{0xD8, 4, "ESPI_BDE"},
	{0xDC, 4, "ESPI_BC"},
};

static const io_register_t alderlake_espi_cfg_registers[] = {
	{0x00, 4, "ESPI_DID_VID"},
	{0x04, 4, "ESPI_STS_CMD"},
	{0x08, 4, "ESPI_CC_RID"},
	{0x2C, 4, "ESPI_SS"},
	{0x34, 4, "ESPI_CAPP"},
	{0x80, 4, "ESPI_IOD_IOE"},
	{0x84, 4, "ESPI_LGIR1"},
	{0x88, 4, "ESPI_LGIR2"},
	{0x8C, 4, "ESPI_LGIR3"},
	{0x90, 4, "ESPI_LGIR4"},
	{0x94, 4, "ESPI_ULKMC"},
	{0x98, 4, "ESPI_LGMR"},
	{0xA0, 4, "ESPI_CS1IORE"},
	{0xA4, 4, "ESPI_CS1GIR1"},
	{0xA8, 4, "ESPI_CS1GMR1"},
	{0xD8, 4, "ESPI_BDE"},
	{0xDC, 4, "ESPI_BC"},
};

static const io_register_t elkhart_espi_cfg_registers[] = {
	{0x00, 4, "ESPI_DID_VID"},
	{0x04, 4, "ESPI_STS_CMD"},
	{0x08, 4, "ESPI_CC_RID"},
	{0x0C, 4, "ESPI_BIST_HTYPE_PLT_CLS"},
	{0x2C, 4, "ESPI_SS"},
	{0x34, 4, "ESPI_CAPP"},
	{0x80, 4, "ESPI_IOD_IOE"},
	{0x84, 4, "ESPI_LGIR1"},
	{0x88, 4, "ESPI_LGIR2"},
	{0x8C, 4, "ESPI_LGIR3"},
	{0x90, 4, "ESPI_LGIR4"},
	{0x94, 4, "ESPI_ULKMC"},
	{0xA0, 4, "ESPI_CS1IORE"},
	{0xA4, 4, "ESPI_CS1GIR1"},
	{0xA8, 4, "ESPI_CS1GMR1"},
	{0xD0, 4, "ESPI_FS1"},
	{0xD4, 4, "ESPI_FS2"},
	{0xD8, 4, "ESPI_BDE"},
	{0xDC, 4, "ESPI_BC"},
};

int print_lpc(struct pci_dev *sb, struct pci_access *pacc)
{
	size_t i, cfg_registers_size = 0;
	const io_register_t *cfg_registers;
	struct pci_dev *dev = NULL;
	uint32_t bc;

	printf("\n========== LPC/eSPI =========\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_H110:
	case PCI_DEVICE_ID_INTEL_H170:
	case PCI_DEVICE_ID_INTEL_Z170:
	case PCI_DEVICE_ID_INTEL_Q170:
	case PCI_DEVICE_ID_INTEL_Q150:
	case PCI_DEVICE_ID_INTEL_B150:
	case PCI_DEVICE_ID_INTEL_C236:
	case PCI_DEVICE_ID_INTEL_C232:
	case PCI_DEVICE_ID_INTEL_QM170:
	case PCI_DEVICE_ID_INTEL_HM170:
	case PCI_DEVICE_ID_INTEL_CM236:
	case PCI_DEVICE_ID_INTEL_HM175:
	case PCI_DEVICE_ID_INTEL_QM175:
	case PCI_DEVICE_ID_INTEL_CM238:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_PRE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_BASE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_PREM:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_IHDCP_PREM:
		dev = pci_get_dev(pacc, sb->domain, sb->bus, sb->dev, 0);
		if (!dev) {
			printf("LPC/eSPI interface not found.\n");
			return 1;
		}
		bc = pci_read_long(dev, SUNRISE_LPC_BC);
		if (bc & (1 << 2)) {
			printf("Device 0:1f.0 is eSPI (BC.LPC_ESPI=1)\n\n");
			cfg_registers = sunrise_espi_cfg_registers;
			cfg_registers_size = ARRAY_SIZE(sunrise_espi_cfg_registers);

		} else {
			printf("Device 0:1f.0 is LPC (BC.LPC_ESPI=0)\n\n");
			cfg_registers = sunrise_lpc_cfg_registers;
			cfg_registers_size = ARRAY_SIZE(sunrise_lpc_cfg_registers);
		}
		break;
	case PCI_DEVICE_ID_INTEL_ADL_N:
		dev = pci_get_dev(pacc, sb->domain, sb->bus, sb->dev, 0);
		if (!dev) {
			printf("LPC/eSPI interface not found.\n");
			return 1;
		}
		printf("Device 0:1f.0 is eSPI (BC.LPC_ESPI=1)\n\n");
		cfg_registers = alderlake_espi_cfg_registers;
		cfg_registers_size = ARRAY_SIZE(alderlake_espi_cfg_registers);
		break;
	case PCI_DEVICE_ID_INTEL_EHL:
		dev = pci_get_dev(pacc, sb->domain, sb->bus, sb->dev, 0);
		if (!dev) {
			printf("LPC/eSPI interface not found.\n");
			return 1;
		}
		bc = pci_read_long(dev, SUNRISE_LPC_BC);
		if (bc & (1 << 2)) {
			printf("Device 0:1f.0 is eSPI (BC.LPC_ESPI=1)\n\n");
			cfg_registers = elkhart_espi_cfg_registers;
			cfg_registers_size = ARRAY_SIZE(elkhart_espi_cfg_registers);

		} else {
			printf("Device 0:1f.0 is LPC (BC.LPC_ESPI=0)\n\n");
			cfg_registers = sunrise_lpc_cfg_registers;
			cfg_registers_size = ARRAY_SIZE(sunrise_lpc_cfg_registers);
		}
		break;
	default:
		printf("Error: Dumping LPC/eSPI on this southbridge is not (yet) supported.\n");
		return 1;
	}

	for (i = 0; i < cfg_registers_size; i++) {
		switch (cfg_registers[i].size) {
		case 4:
			printf("0x%04x: 0x%08x (%s)\n",
				cfg_registers[i].addr,
				pci_read_long(dev, cfg_registers[i].addr),
				cfg_registers[i].name);
			break;
		case 2:
			printf("0x%04x: 0x%04x     (%s)\n",
				cfg_registers[i].addr,
				pci_read_word(dev, cfg_registers[i].addr),
				cfg_registers[i].name);
			break;
		case 1:
			printf("0x%04x: 0x%02x       (%s)\n",
				cfg_registers[i].addr,
				pci_read_byte(dev, cfg_registers[i].addr),
				cfg_registers[i].name);
			break;
		default:
			printf("Error: register size %d not implemented.\n",
				cfg_registers[i].size);
			break;
		}
	}

	if (dev)
		pci_free_dev(dev);

	return 0;
}
