/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	m_cfg->BoardId = 0x1d;
	m_cfg->BoardTypeBitmask = 0x11111111;
	m_cfg->DebugPrintLevel = 8;
	m_cfg->KtiLinkSpeedMode = 0;
	m_cfg->KtiPrefetchEn = 2;
}
