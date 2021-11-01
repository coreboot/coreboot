/* SPDX-License-Identifier: GPL-2.0-only */

#include "../../variant.h"

void variant_silicon_init_params(FSP_S_CONFIG *params)
{
	// PEG0 Config
	params->CpuPcieRpAdvancedErrorReporting[0] = 0;
	params->CpuPcieRpLtrEnable[0] = 1;
	params->CpuPcieRpPtmEnabled[0] = 0;

	// PEG2 Config
	params->CpuPcieRpAdvancedErrorReporting[2] = 0;
	params->CpuPcieRpLtrEnable[2] = 1;
	params->CpuPcieRpPtmEnabled[2] = 0;

	// Remap PEG2 as PEG1
	params->CpuPcieRpFunctionSwap = 1;
}
