/* SPDX-License-Identifier: GPL-2.0-only */

#include "../../variant.h"
#include <soc/pch.h>

void variant_memory_init_params(FSPM_UPD *mupd)
{
	// Enable M.2 PCIE 4.0 and PEG1
	mupd->FspmConfig.CpuPcieRpEnableMask = 0x3;

	// B variant uses Intel GbE
	if (CONFIG(BOARD_SYSTEM76_GAZE16_3060_B))
		mupd->FspmConfig.PcieClkSrcUsage[8] = PCIE_CLK_LAN;
	else
		mupd->FspmConfig.PcieClkSrcUsage[8] = 4;
}
