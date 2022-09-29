/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	// PEG0 Config
	params->CpuPcieRpAdvancedErrorReporting[0] = 0;
	params->CpuPcieRpLtrEnable[0] = 1;
	params->CpuPcieRpPtmEnabled[0] = 0;

	// PEG1 Config
	params->CpuPcieRpAdvancedErrorReporting[1] = 0;
	params->CpuPcieRpLtrEnable[1] = 1;
	params->CpuPcieRpPtmEnabled[1] = 0;

	// IOM config
	params->PchUsbOverCurrentEnable = 0;
	params->PortResetMessageEnable[8] = 1; // TYPEC1
	params->UsbTcPortEn = 1;

	// Low latency legacy I/O
	params->PchLegacyIoLowLatency = 1;
}
