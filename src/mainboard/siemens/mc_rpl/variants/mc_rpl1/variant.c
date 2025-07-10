/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/api.h>
#include <baseboard/variants.h>

void variant_configure_fspm(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *m_cfg = &memupd->FspmConfig;

	m_cfg->ActiveCoreCount = 4;
	m_cfg->ActiveSmallCoreCount = 0;
}
