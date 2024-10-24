/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include "gpio.h"

static void init_mainboard(void *chip_info)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

struct chip_operations mainboard_ops = {
	.init = init_mainboard,
};

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	// PEG0 - Gen4 NVME
	params->CpuPcieRpSlotImplemented[0] = 1;
	params->CpuPcieRpPeerToPeerMode[0] = 1;
	params->CpuPcieRpAcsEnabled[0] = 1;

	// PEG1 - PCI-E x16
	params->CpuPcieRpSlotImplemented[1] = 1;
	params->CpuPcieRpPeerToPeerMode[1] = 1;
	params->CpuPcieRpAcsEnabled[1] = 1;

	// PCH RootPorts
	params->PcieRpAcsEnabled[4] = 1;  // M.2 Gen3
	params->PcieRpAcsEnabled[8] = 1;  // M.2 NGFF
	params->PcieRpAcsEnabled[10] = 1; // RTL8111 NIC
	params->PcieRpAcsEnabled[11] = 1; // PCI-E x1 Gen3

	// Power management: Force-disable ASPM
	params->CpuPciePowerGating = 0;
	params->CpuPcieClockGating = 0;
	params->PchLegacyIoLowLatency = 1;
	params->PchDmiAspmCtrl = 0;

	params->CpuPcieRpEnableCpm[0] = 0;
	params->CpuPcieRpAspm[0] = 0;
	params->CpuPcieRpL1Substates[0] = 0;

	params->CpuPcieRpEnableCpm[1] = 0;
	params->CpuPcieRpAspm[1] = 0;
	params->CpuPcieRpL1Substates[1] = 0;

	params->PcieRpEnableCpm[4] = 0;
	params->PcieRpAspm[4] = 0;
	params->PcieRpL1Substates[4] = 0;

	params->PcieRpEnableCpm[8] = 0;
	params->PcieRpAspm[8] = 0;
	params->PcieRpL1Substates[8] = 0;

	params->PcieRpEnableCpm[10] = 0;
	params->PcieRpAspm[10] = 0;
	params->PcieRpL1Substates[10] = 0;

	params->PcieRpEnableCpm[11] = 0;
	params->PcieRpAspm[11] = 0;
	params->PcieRpL1Substates[11] = 0;
}
