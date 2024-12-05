/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <intelblocks/rtc.h>
#include <soc/romstage.h>
#include <soc/soc_util.h>
#include <static.h>

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

uint8_t get_error_correction_type(const uint8_t RasModesEnabled)
{
	switch (RasModesEnabled) {
	case CH_INDEPENDENT:
		return MEMORY_ARRAY_ECC_SINGLE_BIT;
	case FULL_MIRROR_1LM:
	case PARTIAL_MIRROR_1LM:
	case FULL_MIRROR_2LM:
	case PARTIAL_MIRROR_2LM:
		return MEMORY_ARRAY_ECC_MULTI_BIT;
	case RK_SPARE:
		return MEMORY_ARRAY_ECC_SINGLE_BIT;
	case CH_LOCKSTEP:
		return MEMORY_ARRAY_ECC_SINGLE_BIT;
	default:
		return MEMORY_ARRAY_ECC_MULTI_BIT;
	}
}

uint32_t get_max_capacity_mib(void)
{
	/* According to Dear Customer Letter it's 1.12 TB per processor. */
	return 1.12 * MiB * CONFIG_MAX_SOCKET;
}

uint8_t get_max_dimm_count(void)
{
	return MAX_DIMM;
}

uint8_t get_dram_type(const struct SystemMemoryMapHob *hob)
{
	return MEMORY_TYPE_DDR4;
}
