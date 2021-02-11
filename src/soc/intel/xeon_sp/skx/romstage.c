/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <intelblocks/rtc.h>
#include <soc/romstage.h>
#include <soc/soc_util.h>

#include "chip.h"

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const config_t *config = config_of_soc();
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	mupd->FspmUpdVersion = FSP_UPD_VERSION;

	// ErrorLevel - 0 (disable) to 8 (verbose)
	m_cfg->PcdFspMrcDebugPrintErrorLevel = 0;
	m_cfg->PcdFspKtiDebugPrintErrorLevel = 0;

	mainboard_memory_init_params(mupd);

	m_cfg->VTdConfig.VTdSupport = config->vtd_support;
	m_cfg->VTdConfig.CoherencySupport = config->coherency_support;
	m_cfg->VTdConfig.ATS = config->ats_support;
}
