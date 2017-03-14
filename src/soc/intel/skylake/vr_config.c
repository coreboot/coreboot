/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <fsp/api.h>
#include <soc/ramstage.h>
#include <soc/vr_config.h>

/* Default values for domain configuration. PSI3 and PSI4 are disabled. */
static const struct vr_config default_configs[NUM_VR_DOMAINS] = {
	[VR_SYSTEM_AGENT] = {
		.vr_config_enable = 1,
		.psi1threshold = VR_CFG_AMP(20),
		.psi2threshold = VR_CFG_AMP(4),
		.psi3threshold = VR_CFG_AMP(1),
		.psi3enable = 0,
		.psi4enable = 0,
		.imon_slope = 0x0,
		.imon_offset = 0x0,
		.icc_max = VR_CFG_AMP(7),
		.voltage_limit = 1520,
	},
	[VR_IA_CORE] = {
		.vr_config_enable = 1,
		.psi1threshold = VR_CFG_AMP(20),
		.psi2threshold = VR_CFG_AMP(5),
		.psi3threshold = VR_CFG_AMP(1),
		.psi3enable = 0,
		.psi4enable = 0,
		.imon_slope = 0x0,
		.imon_offset = 0x0,
		.icc_max = VR_CFG_AMP(34),
		.voltage_limit = 1520,
	},
#if IS_ENABLED(CONFIG_PLATFORM_USES_FSP1_1)
	[VR_RING] = {
		.vr_config_enable = 1,
		.psi1threshold = VR_CFG_AMP(20),
		.psi2threshold = VR_CFG_AMP(5),
		.psi3threshold = VR_CFG_AMP(1),
		.psi3enable = 0,
		.psi4enable = 0,
		.imon_slope = 0x0,
		.imon_offset = 0x0,
		.icc_max = VR_CFG_AMP(34),
		.voltage_limit = 1520,
	},
#endif
	[VR_GT_UNSLICED] = {
		.vr_config_enable = 1,
		.psi1threshold = VR_CFG_AMP(20),
		.psi2threshold = VR_CFG_AMP(5),
		.psi3threshold = VR_CFG_AMP(1),
		.psi3enable = 0,
		.psi4enable = 0,
		.imon_slope = 0x0,
		.imon_offset = 0x0,
		.icc_max = VR_CFG_AMP(35),
		.voltage_limit = 1520,
	},
	[VR_GT_SLICED] = {
		.vr_config_enable = 1,
		.psi1threshold = VR_CFG_AMP(20),
		.psi2threshold = VR_CFG_AMP(5),
		.psi3threshold = VR_CFG_AMP(1),
		.psi3enable = 0,
		.psi4enable = 0,
		.imon_slope = 0x0,
		.imon_offset = 0x0,
		.icc_max = VR_CFG_AMP(35),
		.voltage_limit = 1520,
	},
};

void fill_vr_domain_config(void *params,
		int domain, const struct vr_config *chip_cfg)
{
	FSP_SIL_UPD *vr_params = (FSP_SIL_UPD *)params;
	const struct vr_config *cfg;

	if (domain < 0 || domain >= NUM_VR_DOMAINS)
		return;

	/* Use device tree override if requested. */
	if (chip_cfg->vr_config_enable)
		cfg = chip_cfg;
	else
		cfg = &default_configs[domain];

	vr_params->VrConfigEnable[domain] = cfg->vr_config_enable;
	vr_params->Psi1Threshold[domain] = cfg->psi1threshold;
	vr_params->Psi2Threshold[domain] = cfg->psi2threshold;
	vr_params->Psi3Threshold[domain] = cfg->psi3threshold;
	vr_params->Psi3Enable[domain] = cfg->psi3enable;
	vr_params->Psi4Enable[domain] = cfg->psi4enable;
	vr_params->ImonSlope[domain] = cfg->imon_slope;
	vr_params->ImonOffset[domain] = cfg->imon_offset;
	vr_params->IccMax[domain] = cfg->icc_max;
	vr_params->VrVoltageLimit[domain] = cfg->voltage_limit;

#if IS_ENABLED(CONFIG_PLATFORM_USES_FSP2_0)
	vr_params->AcLoadline[domain] = cfg->ac_loadline;
	vr_params->DcLoadline[domain] = cfg->dc_loadline;
#endif
}
