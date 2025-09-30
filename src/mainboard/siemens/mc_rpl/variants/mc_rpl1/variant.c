/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <fsp/api.h>

void variant_configure_fspm(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *m_cfg = &memupd->FspmConfig;

	m_cfg->ActiveCoreCount = 4;
	m_cfg->ActiveSmallCoreCount = 0;
}
