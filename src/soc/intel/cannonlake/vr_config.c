/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <fsp/api.h>
#include <soc/ramstage.h>
#include <soc/vr_config.h>
#include <console/console.h>
#include <intelblocks/cpulib.h>

static const struct vr_config default_configs[NUM_VR_DOMAINS] = {
	[VR_SYSTEM_AGENT] = {
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
	const uint16_t tdp = cpu_get_power_max();
	config_t *cfg = config_of_soc();

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
	 * Iccmax table from Doc #337344 Section 7.2 DC Specifications for CFL.
	 * Iccmax table from Doc #338023 Section 7.2 DC Specifications for WHL.
	 * Iccmax table from Doc #606599 Section 7.2 DC Specifications for CML.
	 *
	 *   Platform             Segment           SA       IA      GT (GT/GTx)
	 * ---------------------------------------------------------------------
	 *  CFL-U                 (28W) GT3 quad   8.5       64       64
	 *  CFL-U                 (28W) GT3 dual   8.5       64       64
	 *
	 *  CFL-H                 (45W) GT2 hex    11.1      128      0
	 *  CFL-H                 (45W) GT2 quad   11.1      86       0
	 *
	 *  CFL-S                 (95W) GT2 octa   11.1      193      45
	 *
	 *  CFL-S                 (95W) GT2 hex    11.1      138      45
	 *  CFL-S                 (65W) GT2 hex    11.1      133      45
	 *  CFL-S                 (80W) GT2 hex    11.1      133      45
	 *  CFL-S                 (35W) GT2 hex    11.1      104      35
	 *
	 *  CFL-S                 (91W) GT2 quad   11.1      100      45
	 *  CFL-S                 (83W) GT2 quad   11.1      100      45
	 *  CFL-S                 (71W) GT2 quad   11.1      100      45
	 *  CFL-S                 (65W) GT2 quad   11.1      79       45
	 *  CFL-S                 (62W) GT2 quad   11.1      79       45
	 *  CFL-S                 (35W) GT2 quad   11.1      66       35
	 *
	 *  CFL-S                 (58W) GT2 dual   11.1      79       45
	 *  CFL-S                 (54W) GT2 dual   11.1      58       45
	 *  CFL-S                 (35W) GT2 dual   11.1      40       35
	 *
	 *  CNL-U                 (15W)            13        34        0
	 *
	 *  WHL-U                 (15W) GT2 quad   6         70       31
	 *  WHL-U                 (15W) GT2 dual   6         35       31
	 *
	 *  CML-U v1/v2           (15W) GT2 hex    6         85(70)   31
	 *  CML-U v1/v2           (15W) GT2 quad   6         85(70)   31
	 *  CML-U v1/v2           (15W) GT2 dual   6         35       31
	 *
	 *  CML-H                 (65W) GT2 octa   11.1      192(165) 32
	 *  CML-H                 (45W) GT2 octa   11.1      165(140) 32
	 *  CML-H                 (45W) GT2 hex    11.1      140(128) 32
	 *  CML-H                 (45W) GT2 quad   11.1      105(86)  32
	 *
	 *  CML-S                 (125W)GT2 deca   11.1      245(210) 35
	 *  CML-S                 (125W)GT2 octa   11.1      245(210) 35
	 *  CML-S                 (125W)GT2 hex    11.1      140      35
	 *  CML-S XeonW           (80W) GT2 deca   11.1      210      35
	 *  CML-S XeonW           (80W) GT2 octa   11.1      210      35
	 *  CML-S XeonW           (80W) GT2 hex    11.1      140      35
	 *  CML-S                 (65W) GT2 deca   11.1      210(175) 35
	 *  CML-S                 (65W) GT2 octa   11.1      210(175) 35
	 *  CML-S                 (65W) GT2 hex    11.1      140      35
	 *  CML-S                 (35W) GT2 deca   11.1      140(104) 35
	 *  CML-S                 (35W) GT2 octa   11.1      140(104) 35
	 *  CML-S                 (35W) GT2 hex    11.1      104      35
	 *
	 *  GT0 versions are the same as GT2/GT3, but have GT/GTx set to 0.
	 *  The above values in () are for baseline.
	 */

	if (igd_id == 0xffff && ((domain == VR_GT_SLICED) || (domain == VR_GT_UNSLICED)))
		return 0;

	switch (mch_id) {
	case PCI_DEVICE_ID_INTEL_CNL_ID_U: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CNL_ID_Y: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(13, 34, 0, 0);

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_WHL_ID_W_4: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(6, 70, 31, 31);

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_WHL_ID_W_2: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(6, 35, 31, 31);

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CFL_ID_U: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_U_2: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(8.5, 64, 64, 64);

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CFL_ID_H_8: /* fallthrough - undocumented */
	case PCI_DEVICE_ID_INTEL_CFL_ID_H: { /* 6 core */
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 128, 0, 0);

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CFL_ID_H_4: { /* 4 core */
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 86, 0, 0);

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_DT_2: { /* 2 core */
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 40, 35, 35);

		if (tdp >= 54) {
			if (tdp >= 58)
				icc_max[VR_IA_CORE] = VR_CFG_AMP(79);
			else
				icc_max[VR_IA_CORE] = VR_CFG_AMP(58);

			icc_max[VR_GT_SLICED] = VR_CFG_AMP(45);
			icc_max[VR_GT_UNSLICED] = VR_CFG_AMP(45);
		}

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_DT_8: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_S_8: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 193, 45, 45);

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CFL_ID_S: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_WS_6: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_S_6: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35);
		if (tdp >= 54) {
			if (tdp >= 95)
				icc_max[VR_IA_CORE] = VR_CFG_AMP(138);
			else if (tdp >= 65)
				icc_max[VR_IA_CORE] = VR_CFG_AMP(133);

			icc_max[VR_GT_SLICED] = VR_CFG_AMP(45);
			icc_max[VR_GT_UNSLICED] = VR_CFG_AMP(45);
		}

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_S_4: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_WS_4: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_DT_4: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 66, 35, 35);
		if (tdp >= 54) {
			if (tdp >= 71)
				icc_max[VR_IA_CORE] = VR_CFG_AMP(100);
			else if (tdp >= 62)
				icc_max[VR_IA_CORE] = VR_CFG_AMP(79);

			icc_max[VR_GT_SLICED] = VR_CFG_AMP(45);
			icc_max[VR_GT_UNSLICED] = VR_CFG_AMP(45);
		}

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_ULT: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CML_ULT_6_2: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(6, 85, 31, 31);
		if (cfg->cpu_pl2_4_cfg == baseline)
			icc_max[VR_IA_CORE] = VR_CFG_AMP(70);

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_ULT_2_2: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(6, 35, 31, 31);

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_H_8_2: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 192, 32, 32);
		if (tdp >= 65) { /* 65W */
			if (cfg->cpu_pl2_4_cfg == baseline)
				icc_max[VR_IA_CORE] = VR_CFG_AMP(165);
			else
				icc_max[VR_IA_CORE] = VR_CFG_AMP(192);
		} else { /* 45W */
			if (cfg->cpu_pl2_4_cfg == baseline)
				icc_max[VR_IA_CORE] = VR_CFG_AMP(140);
			else
				icc_max[VR_IA_CORE] = VR_CFG_AMP(165);
		}
		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_H: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 140, 32, 32);

		if (cfg->cpu_pl2_4_cfg == baseline)
			icc_max[VR_IA_CORE] = VR_CFG_AMP(128);

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_H_4_2: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 105, 32, 32);

		if (cfg->cpu_pl2_4_cfg == baseline)
			icc_max[VR_IA_CORE] = VR_CFG_AMP(86);

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_S_P0P1_8_2: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CML_S_P0P1_10_2: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 140, 35, 35);
		if (tdp >= 125) /* 125W */
			if (cfg->cpu_pl2_4_cfg == baseline)
				icc_max[VR_IA_CORE] = VR_CFG_AMP(210);
			else
				icc_max[VR_IA_CORE] = VR_CFG_AMP(245);
		else if (tdp >= 80) /* 80W */
			icc_max[VR_IA_CORE] = VR_CFG_AMP(210);
		else if (tdp >= 65) /* 65W */
			if (cfg->cpu_pl2_4_cfg == baseline)
				icc_max[VR_IA_CORE] = VR_CFG_AMP(175);
			else
				icc_max[VR_IA_CORE] = VR_CFG_AMP(210);
		else /* 35W */
			if (cfg->cpu_pl2_4_cfg == baseline)
				icc_max[VR_IA_CORE] = VR_CFG_AMP(104);
			else
				icc_max[VR_IA_CORE] = VR_CFG_AMP(140);

		return icc_max[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_S_G0G1_P0P1_6_2: {
		uint16_t icc_max[NUM_VR_DOMAINS] = VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35);
		if (tdp >= 65) /* 125W or 80W or 65W */
			icc_max[VR_IA_CORE] = VR_CFG_AMP(140);
		else /* 35W */
			icc_max[VR_IA_CORE] = VR_CFG_AMP(104);

		return icc_max[domain];
	}
	default:
		printk(BIOS_ERR, "ERROR: Unknown MCH (0x%x) in %s\n", mch_id, __func__);
	}
	return 0;
}

static uint16_t get_sku_ac_dc_loadline(const int domain)
{
	const uint16_t tdp = cpu_get_power_max();
	static uint16_t mch_id = 0;
	if (!mch_id) {
		struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
		mch_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;
	}

	switch (mch_id) {
	case PCI_DEVICE_ID_INTEL_WHL_ID_W_4: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_H_8: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_H: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_H_4: { /* fallthrough */
		uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.8, 2.7, 2.7);
		if (mch_id == PCI_DEVICE_ID_INTEL_WHL_ID_W_4) {
			loadline[VR_GT_SLICED] = 0; /* unspecified */
			loadline[VR_GT_UNSLICED] = 0; /* unspecified */
		}
		return loadline[domain];
	}
	case PCI_DEVICE_ID_INTEL_WHL_ID_W_2: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_U: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_U_2: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CNL_ID_U: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CNL_ID_Y: {
		uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.4, 2.0, 2.0);
		if (mch_id == PCI_DEVICE_ID_INTEL_WHL_ID_W_2) {
			loadline[VR_GT_SLICED] = 0; /* unspecified */
			loadline[VR_GT_UNSLICED] = 0; /* unspecified */
		}
		return loadline[domain];
	}
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_DT_8: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_S_8: {
		/* FIXME: Loadline isn't specified for S-series, using H-series default */
		const uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.6, 3.1, 3.1);
		return loadline[domain];
	}
	case PCI_DEVICE_ID_INTEL_CFL_ID_S: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_WS_6: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_S_6: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_S_4: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_WS_4: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_DT_4: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CFL_ID_S_DT_2: {
		/* FIXME: Loadline isn't specified for S-series, using H-series default */
		const uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.1, 3.1, 3.1);
		return loadline[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_ULT_2_2: {
		const uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.4, 3.1, 3.1);
		return loadline[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_ULT: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CML_ULT_6_2: {
		const uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.8, 3.1, 3.1);
		return loadline[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_H_4_2: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CML_H: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CML_H_8_2: {
		const uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.1, 2.7, 2.7);
		return loadline[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_S_G0G1_P0P1_6_2: {
		uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.7, 4.0, 4.0);
		if (tdp >= 125)
			loadline[VR_IA_CORE] = VR_CFG_MOHMS(1.1);
		return loadline[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_S_P0P1_8_2: /* fallthrough */
	case PCI_DEVICE_ID_INTEL_CML_S_P0P1_10_2: {
		uint16_t loadline[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.7, 4.0, 4.0);
		if (tdp > 35)
			loadline[VR_IA_CORE] = VR_CFG_MOHMS(1.1);
		return loadline[domain];
	}
	default:
		printk(BIOS_ERR, "ERROR: Unknown MCH (0x%x) in %s\n", mch_id, __func__);
	}
	return 0;
}

static uint16_t get_sku_voltagelimit(int domain)
{
	return 1520;
}

static uint16_t get_sku_tdc_powerlimit(int domain)
{
	const uint16_t tdp = cpu_get_power_max();
	const config_t *cfg = config_of_soc();

	static uint16_t mch_id = 0;
	if (!mch_id) {
		struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
		mch_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;
	}

	switch (mch_id) {
	case PCI_DEVICE_ID_INTEL_CML_ULT:
	case PCI_DEVICE_ID_INTEL_CML_ULT_6_2: {
		uint16_t tdc[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_TDC(4, 58, 22, 22);

		if (cfg->cpu_pl2_4_cfg == baseline)
			tdc[VR_IA_CORE] = VR_CFG_TDC_AMP(48);

		return tdc[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_ULT_2_2: {
		const uint16_t tdc[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_TDC(4, 24, 22, 22);
		return tdc[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_H_4_2: {
		uint16_t tdc[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_TDC(10, 80, 25, 25);

		if (cfg->cpu_pl2_4_cfg == baseline)
			tdc[VR_IA_CORE] = VR_CFG_TDC_AMP(60);

		return tdc[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_H: {
		uint16_t tdc[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_TDC(10, 92, 25, 25);

		if (cfg->cpu_pl2_4_cfg == baseline)
			tdc[VR_IA_CORE] = VR_CFG_TDC_AMP(80);

		return tdc[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_H_8_2: {
		uint16_t tdc[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_TDC(10, 125, 25, 25);

		if (tdp >= 65) /* 65W */
			tdc[VR_IA_CORE] = (cfg->cpu_pl2_4_cfg == baseline) ?
					  VR_CFG_TDC_AMP(117) :
					  VR_CFG_TDC_AMP(146);
		else /* 45W */
			tdc[VR_IA_CORE] = (cfg->cpu_pl2_4_cfg == baseline) ?
					  VR_CFG_TDC_AMP(86) :
					  VR_CFG_TDC_AMP(125);

		return tdc[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_S_G0G1_P0P1_6_2: {
		uint16_t tdc[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_TDC(10, 74, 28, 28);

		if (tdp >= 125) /* 125W */
			tdc[VR_IA_CORE] = VR_CFG_TDC_AMP(132);
		else if (tdp >= 65) /* 80W or 65W */
			tdc[VR_IA_CORE] = VR_CFG_TDC_AMP(104);
		else /* 35W */
			tdc[VR_IA_CORE] = VR_CFG_TDC_AMP(74);

		return tdc[domain];
	}
	case PCI_DEVICE_ID_INTEL_CML_S_P0P1_8_2:
	case PCI_DEVICE_ID_INTEL_CML_S_P0P1_10_2: {
		uint16_t tdc[NUM_VR_DOMAINS] =
				VR_CFG_ALL_DOMAINS_TDC(10, 100, 28, 28);

		if (tdp > 35) /* 125W or 80W or 65W */
			tdc[VR_IA_CORE] = VR_CFG_TDC_AMP(175);

		return tdc[domain];
	}
	default:
		printk(BIOS_ERR, "ERROR: Unknown MCH (0x%x) in %s\n", mch_id, __func__);
	}

	return 0;
}

void fill_vr_domain_config(void *params,
		int domain, const struct vr_config *chip_cfg)
{
	FSP_S_CONFIG *vr_params = (FSP_S_CONFIG *)params;
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
	if (cfg->voltage_limit)
		vr_params->VrVoltageLimit[domain] = cfg->voltage_limit;
	else
		vr_params->VrVoltageLimit[domain] = get_sku_voltagelimit(domain);

	if (cfg->icc_max)
		vr_params->IccMax[domain] = cfg->icc_max;
	else
		vr_params->IccMax[domain] = get_sku_icc_max(domain);

	if (cfg->ac_loadline)
		vr_params->AcLoadline[domain] = cfg->ac_loadline;
	else
		vr_params->AcLoadline[domain] = get_sku_ac_dc_loadline(domain);

	if (cfg->dc_loadline)
		vr_params->DcLoadline[domain] = cfg->dc_loadline;
	else
		vr_params->DcLoadline[domain] = get_sku_ac_dc_loadline(domain);

	vr_params->TdcEnable[domain] = !cfg->tdc_disable;

	if (cfg->tdc_powerlimit)
		vr_params->TdcPowerLimit[domain] = cfg->tdc_powerlimit;
	else
		vr_params->TdcPowerLimit[domain] = get_sku_tdc_powerlimit(domain);
}
