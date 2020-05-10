/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	void *start = (void *) m_cfg;

	// BoardId
	write8(start + 140, 0x1d);

	// BoardTypeBitmask
	write32(start + 104, 0x11111111);

	// DebugPrintLevel
	write8(start + 45, 8);

	// KtiLinkSpeedMode
	write8(start + 64, 0);

	// KtiPrefetchEn
	write8(start + 53, 2);
}
