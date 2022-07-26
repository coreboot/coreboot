/* SPDX-License-Identifier: GPL-2.0-only */

#include "../../variant.h"

void variant_memory_init_params(FSPM_UPD *mupd)
{
	// Enable M.2 PCIE 4.0 and PEG2
	mupd->FspmConfig.CpuPcieRpEnableMask = 0x5;
}
