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

#include <arch/io.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <fsp/api.h>
#include <soc/ramstage.h>
#include <soc/vr_config.h>

#define KBLY_ICCMAX_SA					VR_CFG_AMP(4.1)
#define KBLY_ICCMAX_CORE				VR_CFG_AMP(24)
#define AMLY_ICCMAX_CORE				VR_CFG_AMP(28)
#define KBLY_ICCMAX_GTS_GTUS			VR_CFG_AMP(24)
#define KBLR_ICCMAX_SA_U42				VR_CFG_AMP(6)
#define KBLU_ICCMAX_SA_U22				VR_CFG_AMP(4.5)
#define KBLR_ICCMAX_CORE_U42			VR_CFG_AMP(64)
#define KBLU_ICCMAX_CORE_U22_PREMIUM	VR_CFG_AMP(32)
#define KBLU_ICCMAX_CORE_U22_BASE		VR_CFG_AMP(29)
#define KBLUR_ICCMAX_GTS_GTUS			VR_CFG_AMP(31)

enum kbl_sku {
	KBL_Y_SKU,
	KBL_R_SKU,
	KBL_U_BASE_SKU,
	KBL_U_PREMIUM_SKU,
	AML_Y_SKU,
};

/*
 * Iccmax table from Doc #559100 Section 7.2 DC Specifications, the
 * Iccmax is the same among KBL-Y but KBL-U/R.
 * Addendum for AML-Y #594883, IccMax for IA core is 28A.
 * +----------------+-------------+---------------+------+-----+
 * | Domain/Setting |  SA         |  IA           | GTUS | GTS |
 * +----------------+-------------+---------------+------+-----+
 * | IccMax(KBL-U/R)| 6A(U42)     | 64A(U42)      | 31A  | 31A |
 * |                | 4.5A(Others)| 29A(P/C)      |      |     |
 * |                |             | 32A(i3/i5)    |      |     |
 * +----------------+-------------+---------------+------+-----+
 * | IccMax(KBL-Y)  | 4.1A        | 24A           | 24A  | 24A |
 * +----------------+-------------+---------------+------+-----+
 * | IccMax(AML-Y)  | 4.1A        | 28A           | 24A  | 24A |
 * +----------------+-------------+---------------+------+-----+
 */

static const struct {
	uint16_t icc_max[NUM_VR_DOMAINS];
}sku_icc_max_mapping[] = {
	[KBL_Y_SKU] = {
		.icc_max = {
			KBLY_ICCMAX_SA,
			KBLY_ICCMAX_CORE,
			KBLY_ICCMAX_GTS_GTUS,
			KBLY_ICCMAX_GTS_GTUS
		}
	},
	[KBL_R_SKU] = {
		.icc_max = {
			KBLR_ICCMAX_SA_U42,
			KBLR_ICCMAX_CORE_U42,
			KBLUR_ICCMAX_GTS_GTUS,
			KBLUR_ICCMAX_GTS_GTUS
		}
	},
	[KBL_U_BASE_SKU] = {
		.icc_max = {
			KBLU_ICCMAX_SA_U22,
			KBLU_ICCMAX_CORE_U22_BASE,
			KBLUR_ICCMAX_GTS_GTUS,
			KBLUR_ICCMAX_GTS_GTUS
		}
	},
	[KBL_U_PREMIUM_SKU] = {
		.icc_max = {
			KBLU_ICCMAX_SA_U22,
			KBLU_ICCMAX_CORE_U22_PREMIUM,
			KBLUR_ICCMAX_GTS_GTUS,
			KBLUR_ICCMAX_GTS_GTUS
		}
	},
	[AML_Y_SKU] = {
		.icc_max = {
			KBLY_ICCMAX_SA,
			AMLY_ICCMAX_CORE,
			KBLY_ICCMAX_GTS_GTUS,
			KBLY_ICCMAX_GTS_GTUS
		}
	},
};

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

static uint16_t get_dev_id(struct device *dev)
{
	return pci_read_config16(dev, PCI_DEVICE_ID);
}

static int get_kbl_sku(void)
{
	static int sku = -1;
	uint16_t id;

	if (sku != -1)
		return sku;

	id = get_dev_id(SA_DEV_ROOT);
	if (id == PCI_DEVICE_ID_INTEL_KBL_U_R)
		sku = KBL_R_SKU;
	else if (id == PCI_DEVICE_ID_INTEL_KBL_ID_Y) {
		id = get_dev_id(SA_DEV_IGD);
		if (id == PCI_DEVICE_ID_INTEL_KBL_GT2_ULX_R)
			sku = AML_Y_SKU;
		else
			sku = KBL_Y_SKU;
	} else if (id == PCI_DEVICE_ID_INTEL_KBL_ID_U) {
		id = get_dev_id(PCH_DEV_LPC);
		if (id == PCI_DEVICE_ID_INTEL_SPT_LP_U_BASE_HDCP22)
			sku = KBL_U_BASE_SKU;
		else
			sku = KBL_U_PREMIUM_SKU;
	} else
		/* Not one of the skus with available Icc max mapping. */
		sku = -2;
	return sku;
}

static uint16_t get_sku_icc_max(int domain, uint16_t board_icc_max)
{
	/* If board provided non-zero value, use it. */
	if (board_icc_max)
		return board_icc_max;

	/* Check if this SKU has a mapping table entry. */
	int sku_id = get_kbl_sku();
	if (sku_id < 0)
		return 0;
	return sku_icc_max_mapping[sku_id].icc_max[domain];
}

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
	vr_params->IccMax[domain] = get_sku_icc_max(domain, cfg->icc_max);
	vr_params->VrVoltageLimit[domain] = cfg->voltage_limit;

#if IS_ENABLED(CONFIG_PLATFORM_USES_FSP2_0)
	vr_params->AcLoadline[domain] = cfg->ac_loadline;
	vr_params->DcLoadline[domain] = cfg->dc_loadline;
#endif
}
