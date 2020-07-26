/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	m_cfg->KtiLinkSpeedMode = 0;
}
