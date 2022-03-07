/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <fsp/api.h>
#include <soc/ramstage.h>
#include <soc/vr_config.h>
#include <console/console.h>
#include <intelblocks/cpulib.h>

/* Default values for domain configuration. */
static const struct vr_config default_configs[NUM_VR_DOMAINS] = {
	[VR_SYSTEM_AGENT] = {
		.vr_config_enable = 1,
		.psi1threshold = VR_CFG_AMP(20),
		.psi2threshold = VR_CFG_AMP(4),
		.psi3threshold = VR_CFG_AMP(1),
		.psi3enable = 1,
		.psi4enable = 1,
		.imon_slope = 0,
		.imon_offset = 0,
		.icc_max = 0,
		.voltage_limit = 1520,
	},
	[VR_IA_CORE] = {
		.vr_config_enable = 1,
		.psi1threshold = VR_CFG_AMP(20),
		.psi2threshold = VR_CFG_AMP(5),
		.psi3threshold = VR_CFG_AMP(1),
		.psi3enable = 1,
		.psi4enable = 1,
		.imon_slope = 0,
		.imon_offset = 0,
		.icc_max = 0,
		.voltage_limit = 1520,
	},
	[VR_GT_UNSLICED] = {
		.vr_config_enable = 1,
		.psi1threshold = VR_CFG_AMP(20),
		.psi2threshold = VR_CFG_AMP(5),
		.psi3threshold = VR_CFG_AMP(1),
		.psi3enable = 1,
		.psi4enable = 1,
		.imon_slope = 0,
		.imon_offset = 0,
		.icc_max = 0,
		.voltage_limit = 1520,
	},
	[VR_GT_SLICED] = {
		.vr_config_enable = 1,
		.psi1threshold = VR_CFG_AMP(20),
		.psi2threshold = VR_CFG_AMP(5),
		.psi3threshold = VR_CFG_AMP(1),
		.psi3enable = 1,
		.psi4enable = 1,
		.imon_slope = 0,
		.imon_offset = 0,
		.icc_max = 0,
		.voltage_limit = 1520,
	},
};

static uint16_t get_sku_icc_max(int domain)
{
	const uint16_t tdp = cpu_get_power_max() / 1000;

	static uint16_t mch_id = 0, igd_id = 0;
	if (!mch_id) {
		struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
		mch_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;
	}
	if (!igd_id) {
		struct device *dev = pcidev_path_on_root(SA_DEVFN_IGD);
		igd_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;
	}

	/*
	 * Iccmax table from Doc #559100 Section 7.2 DC Specifications, the
	 * Iccmax is the same among KBL-Y but KBL-U/R.
	 * Addendum for AML-Y #594883, IccMax for IA core is 28A.
	 * KBL-S #335195, KBL-H #335190, SKL-S #332687, SKL-H #332986,
	 * SKL-U/Y #332990
	 *
	 *   Platform             Segment           SA       IA      GT (GT/GTx)
	 * ---------------------------------------------------------------------
	 *   KBL/SKL-S            (95W) quad        11.1     100     45
	 *   SKL-S                (80W) quad        11.1     82      45
	 *   KBL/SKL-S            (65W) quad        11.1     79      45
	 *   SKL-S                (45W) quad        11.1     70      0
	 *   KBL/SKL-S            (35W) quad        11.1     66      35
	 *   SKL-S                (25W) quad        11.1     55      35
	 *
	 *   KBL/SKL-S            (54W) dual        11.1     58      48
	 *   KBL/SKL-S            (51W) dual        11.1     45      48
	 *   KBL/SKL-S            (35W) dual        11.1     40      48
	 *
	 *   SKL-H + OPC          (65W) GT4 quad    8        74      105/24
	 *   SKL-H + OPC          (45W) GT4 quad    8        74      94/20
	 *   SKL-H + OPC          (35W) GT4 quad    8        66      94/20
	 *
	 *   SKL-H                (35W) GT2 dual    11.1     60      55
	 *
	 *   KBL/SKL-H            (45W) GT2 quad    11.1     68      55
	 *   KBL-H                (18W) GT2 quad    6.6      60      55
	 *
	 *   SKL-U + OPC          (28W) GT3 dual    5.1      32      57/19
	 *   SKL-U + OPC          (15W) GT3 dual    5.1      29      57/19
	 *   SKL-U                (15W) GT2 dual    4.5      29      31
	 *
	 *   KBL-U + OPC          (28W) GT3 dual    5.1      32      57/19
	 *   KBL-U + OPC          (15W) GT3 dual    5.1      32      57/19
	 *   KBL-U                (15W) GT1/2 dual  4.5      32      31
	 *   KBL-U [*]            (15W) GT1 quad    4.5      29      31
	 *
	 *   KBL-U/R              (15W) GT2 quad    6        64      31
	 *
	 *   SKL/KBL-Y            (6W)              4.1      24      24
	 *   SKL/KBL-Y            (4.5W)            4.1      24      24
	 *
	 *   [*] Pentium/Celeron CPUs with HD Graphics 610
	 */

	switch (mch_id) {
	case PCI_DID_INTEL_SKL_ID_S_2: /* fallthrough */
	case PCI_DID_INTEL_KBL_ID_S: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 40, 48, 48);
		if (tdp >= 54)
			icc_max[VR_IA_CORE] = VR_CFG_AMP(58);
		else if (tdp >= 51)
			icc_max[VR_IA_CORE] = VR_CFG_AMP(45);

		return icc_max[domain];
	}
	case PCI_DID_INTEL_SKL_ID_S_4:  /* fallthrough */
	case PCI_DID_INTEL_KBL_ID_DT_2: /* fallthrough */
	case PCI_DID_INTEL_KBL_ID_DT: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 55, 45, 45);
		if (tdp >= 91)
			icc_max[VR_IA_CORE] = VR_CFG_AMP(100);
		else if (tdp >= 80)
			icc_max[VR_IA_CORE] = VR_CFG_AMP(82);
		else if (tdp >= 65)
			icc_max[VR_IA_CORE] = VR_CFG_AMP(79);
		else if (tdp >= 45) {
			icc_max[VR_IA_CORE] = VR_CFG_AMP(70);
			icc_max[VR_GT_SLICED] = 0;
			icc_max[VR_GT_UNSLICED] = 0;
		} else if (tdp >= 25) {
			if (tdp >= 35)
				icc_max[VR_IA_CORE] = VR_CFG_AMP(66);

			icc_max[VR_GT_SLICED] = VR_CFG_AMP(35);
			icc_max[VR_GT_UNSLICED] = VR_CFG_AMP(35);
		}

		return icc_max[domain];
	}
	case PCI_DID_INTEL_SKL_ID_H_4: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 60, 94, 20);
		if (tdp >= 45) {
			icc_max[VR_IA_CORE] = VR_CFG_AMP(74);
			if (tdp >= 65) {
				icc_max[VR_GT_SLICED] = VR_CFG_AMP(105);
				icc_max[VR_GT_UNSLICED] = VR_CFG_AMP(24);
			}
		}
		return icc_max[domain];
	}
	case PCI_DID_INTEL_SKL_ID_H_2:  /* fallthrough */
	case PCI_DID_INTEL_SKL_ID_H_EM: /* fallthrough */
	case PCI_DID_INTEL_KBL_ID_H: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(6.6, 60, 55, 55);
		if (tdp >= 35) {
			if (tdp >= 45)
				icc_max[VR_IA_CORE] = VR_CFG_AMP(68);

			icc_max[VR_SYSTEM_AGENT] = VR_CFG_AMP(11.1);
		}

		return icc_max[domain];
	}
	case PCI_DID_INTEL_SKL_ID_U: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(5.1, 29, 57, 19);
		if (tdp >= 28)
			icc_max[VR_IA_CORE] = VR_CFG_AMP(32);
		else if (igd_id != PCI_DID_INTEL_SKL_GT3E_SULTM_1) {
			const uint16_t icc_max_gt2[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_ICC(4.5, 29, 31, 31);

			return icc_max_gt2[domain];
		}
		return icc_max[domain];
	}
	case PCI_DID_INTEL_KBL_U_R: {
		const uint16_t icc_max[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_ICC(6, 64, 31, 31);
		return icc_max[domain];
	}
	case PCI_DID_INTEL_SKL_ID_Y: /* fallthrough */
	case PCI_DID_INTEL_KBL_ID_Y: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(4.1, 24, 24, 24);

		if (igd_id == PCI_DID_INTEL_AML_GT2_ULX)
			icc_max[VR_IA_CORE] = VR_CFG_AMP(28);

		return icc_max[domain];
	}
	case PCI_DID_INTEL_KBL_ID_U: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(4.5, 32, 31, 31);

		if (igd_id == PCI_DID_INTEL_KBL_GT1_SULTM)
			icc_max[VR_IA_CORE] = VR_CFG_AMP(29);

		else if ((igd_id == PCI_DID_INTEL_KBL_GT3E_SULTM_1) ||
			 (igd_id == PCI_DID_INTEL_KBL_GT3E_SULTM_2)) {
			const uint16_t icc_max_gt3[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_ICC(5.1, 32, 57, 19);

			return icc_max_gt3[domain];
		}

		return icc_max[domain];
	}
	default:
		printk(BIOS_ERR, "Unknown MCH (0x%x) in %s\n", mch_id, __func__);
	}
	return 0;
}

static uint16_t get_sku_ac_dc_loadline(const int domain)
{
	static uint16_t mch_id = 0, igd_id = 0;
	if (!mch_id) {
		struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
		mch_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;
	}
	if (!igd_id) {
		struct device *dev = pcidev_path_on_root(SA_DEVFN_IGD);
		igd_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;
	}

	switch (mch_id) {
	case PCI_DID_INTEL_SKL_ID_S_2: /* fallthrough */
	case PCI_DID_INTEL_SKL_ID_S_4: /* fallthrough */
	case PCI_DID_INTEL_KBL_ID_S:   /* fallthrough */
	case PCI_DID_INTEL_KBL_ID_DT:  /* fallthrough */
	case PCI_DID_INTEL_KBL_ID_DT_2: {
		/* SA Loadline is not specified */
		const uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(0, 2.1, 3.1, 3.1);
		return loadline[domain];
	}
	case PCI_DID_INTEL_SKL_ID_H_2:  /* fallthrough */
	case PCI_DID_INTEL_SKL_ID_H_EM: /* fallthrough */
	case PCI_DID_INTEL_SKL_ID_H_4:  /* fallthrough */
	case PCI_DID_INTEL_KBL_ID_H: {
		const uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(10, 1.8, 2.65, 2.65);

		if (igd_id == PCI_DID_INTEL_SKL_GT4_SHALM) {
			const uint16_t loadline_gt4[NUM_VR_DOMAINS] =
					VR_CFG_ALL_DOMAINS_LOADLINE(6, 1.6, 1.4, 6);
			return loadline_gt4[domain];
		}

		return loadline[domain];
	}
	case PCI_DID_INTEL_SKL_ID_Y: /* fallthrough */
	case PCI_DID_INTEL_KBL_ID_Y: {
		uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(18, 5.9, 5.7, 5.7);

		if (igd_id == PCI_DID_INTEL_AML_GT2_ULX)
			loadline[VR_IA_CORE] = VR_CFG_MOHMS(4);

		return loadline[domain];
	}
	case PCI_DID_INTEL_SKL_ID_U: /* fallthrough */
	case PCI_DID_INTEL_KBL_U_R:  /* fallthrough */
	case PCI_DID_INTEL_KBL_ID_U: {
		uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.4, 3.1, 3.1);

		if ((igd_id == PCI_DID_INTEL_SKL_GT3E_SULTM_1) ||
		    (igd_id == PCI_DID_INTEL_SKL_GT3E_SULTM_2) ||
		    (igd_id == PCI_DID_INTEL_KBL_GT3E_SULTM_1) ||
		    (igd_id == PCI_DID_INTEL_KBL_GT3E_SULTM_2)) {
			loadline[VR_GT_UNSLICED] = VR_CFG_MOHMS(2);
			loadline[VR_GT_SLICED] = VR_CFG_MOHMS(6);
		}

		return loadline[domain];
	}
	default:
		printk(BIOS_ERR, "Unknown MCH (0x%x) in %s\n", mch_id, __func__);
	}
	return 0;
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

	/* If board provided non-zero value, use it. */
	if (cfg->icc_max)
		vr_params->IccMax[domain] = cfg->icc_max;
	else
		vr_params->IccMax[domain] = get_sku_icc_max(domain);
	vr_params->VrVoltageLimit[domain] = cfg->voltage_limit;

	if (cfg->ac_loadline)
		vr_params->AcLoadline[domain] = cfg->ac_loadline;
	else
		vr_params->AcLoadline[domain] = get_sku_ac_dc_loadline(domain);
	if (cfg->dc_loadline)
		vr_params->DcLoadline[domain] = cfg->dc_loadline;
	else
		vr_params->DcLoadline[domain] = get_sku_ac_dc_loadline(domain);
}
