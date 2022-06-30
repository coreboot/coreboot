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

struct vr_lookup_item {
	uint16_t tdp_min;
	enum chip_pl2_4_cfg pl2_4_cfg; /* Use 'value_not_set' for don't care */
	uint16_t conf[NUM_VR_DOMAINS];
};

struct vr_lookup {
	uint16_t mchid;
	uint8_t num_items;
	const struct vr_lookup_item *items;
};

#define VR_CONFIG(x, y) \
	static const struct vr_lookup_item vr_config_##x##_##y[] =
#define VR_CONFIG_ICC(x) VR_CONFIG(x, ICC)
#define VR_CONFIG_LL(x) VR_CONFIG(x, LL)
#define VR_CONFIG_TDC(x) VR_CONFIG(x, TDC)

#define VR_REFITEM(x, y) { x, ARRAY_SIZE(vr_config_##x##_##y), vr_config_##x##_##y}
#define VR_REFITEM_ICC(x) VR_REFITEM(x, ICC)
#define VR_REFITEM_LL(x) VR_REFITEM(x, LL)
#define VR_REFITEM_TDC(x) VR_REFITEM(x, TDC)

static uint16_t load_table(const struct vr_lookup *tbl,
			   const int tbl_entries,
			   const int domain,
			   const uint16_t tdp,
			   const uint16_t mch_id)
{
	const config_t *cfg = config_of_soc();

	for (size_t i = 0; i < tbl_entries; i++) {
		if (tbl[i].mchid != mch_id)
			continue;

		for (size_t j = 0; j < tbl[i].num_items; j++) {
			if (tbl[i].items[j].tdp_min > tdp)
				continue;

			if ((tbl[i].items[j].pl2_4_cfg != value_not_set) &&
			    (tbl[i].items[j].pl2_4_cfg != cfg->cpu_pl2_4_cfg))
				continue;

			return tbl[i].items[j].conf[domain];
		}
		break;
	}

	printk(BIOS_ERR, "Unknown MCH (0x%x) in %s\n", mch_id, __func__);

	return 0;
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
 *  CML-S                 (65W) GT2 quad   11.1      102      35
 *  CML-S                 (35W) GT2 quad   11.1      65       35
 *  CML-S                 (58W) GT2 dual   11.1      60       35
 *  CML-S                 (35W) GT2 dual   11.1      55       35
 *
 *  GT0 versions are the same as GT2/GT3, but have GT/GTx set to 0.
 *  The above values in () are for baseline.
 */

VR_CONFIG_ICC(PCI_DID_INTEL_CNL_ID_U) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(13, 34, 0, 0) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CNL_ID_Y) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(13, 34, 0, 0) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_WHL_ID_W_4) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(6, 70, 31, 31) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_WHL_ID_W_2) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(6, 35, 31, 31) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_U) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(8.5, 64, 64, 64) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_U_2) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(8.5, 64, 64, 64) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_H_8) { /* undocumented */
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 128, 0, 0) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_H) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 128, 0, 0) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_H_4) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 86, 0, 0) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_S) {
	{ 95, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 138, 45, 45) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 133, 45, 45) },
	{ 35, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_S_DT_2) {
	{ 58, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 79, 35, 35) },
	{ 54, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 58, 45, 45) },
	{ 35, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 40, 35, 35) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 40, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_S_S_4) {
	{ 83, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 100, 45, 45) },
	{ 71, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 100, 45, 45) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 79, 45, 45) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 66, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_S_WS_4) {
	{ 83, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 100, 45, 45) },
	{ 71, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 100, 45, 45) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 79, 45, 45) },
	{ 35, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 66, 35, 35) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 66, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_S_DT_4) {
	{ 91, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 100, 45, 45) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 79, 45, 45) },
	{ 62, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 79, 45, 45) },
	{ 35, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 66, 35, 35) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 66, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_S_WS_6) {
	{ 95, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 138, 45, 45) },
	{ 80, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 133, 45, 45) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 133, 45, 45) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_S_S_6) {
	{ 95, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 138, 45, 45) },
	{ 80, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 133, 45, 45) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 133, 45, 45) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_S_S_8) {
	{ 95, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 193, 45, 45) },
	{ 80, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 193, 45, 45) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 186, 45, 45) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_S_WS_8) {
	{ 95, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 193, 45, 45) },
	{ 80, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 193, 45, 45) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 186, 45, 45) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CFL_ID_S_DT_8) {
	{ 127, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 193, 45, 45) },
	{ 95, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 193, 45, 45) },
	{ 80, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 193, 45, 45) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 186, 45, 45) },
	{ 35, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35) },
};

VR_CONFIG_ICC(PCI_DID_INTEL_CML_ULT) {
	{  0, performance,  VR_CFG_ALL_DOMAINS_ICC(6, 85, 31, 31) },
	{  0, baseline,     VR_CFG_ALL_DOMAINS_ICC(6, 70, 31, 31) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CML_ULT_6_2) {
	{  0, performance,  VR_CFG_ALL_DOMAINS_ICC(6, 85, 31, 31) },
	{  0, baseline,     VR_CFG_ALL_DOMAINS_ICC(6, 70, 31, 31) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CML_ULT_2_2) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(6, 35, 31, 31) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CML_H_8_2) {
	{ 65, baseline,     VR_CFG_ALL_DOMAINS_ICC(11.1, 165, 32, 32) },
	{ 65, performance,  VR_CFG_ALL_DOMAINS_ICC(11.1, 192, 32, 32) },
	{  0, baseline,     VR_CFG_ALL_DOMAINS_ICC(11.1, 140, 32, 32) },
	{  0, performance,  VR_CFG_ALL_DOMAINS_ICC(11.1, 165, 32, 32) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CML_H) {
	{  0, performance,  VR_CFG_ALL_DOMAINS_ICC(11.1, 140, 32, 32) },
	{  0, baseline,     VR_CFG_ALL_DOMAINS_ICC(11.1, 140, 32, 32) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CML_H_4_2) {
	{  0, performance,  VR_CFG_ALL_DOMAINS_ICC(11.1, 105, 32, 32) },
	{  0, baseline,     VR_CFG_ALL_DOMAINS_ICC(11.1, 86, 32, 32) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CML_S_P0P1_8_2) {
	{125, performance,  VR_CFG_ALL_DOMAINS_ICC(11.1, 245, 35, 35) },
	{125, baseline,     VR_CFG_ALL_DOMAINS_ICC(11.1, 210, 35, 35) },
	{ 80, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 210, 35, 35) },
	{ 65, performance,  VR_CFG_ALL_DOMAINS_ICC(11.1, 210, 35, 35) },
	{ 65, baseline,     VR_CFG_ALL_DOMAINS_ICC(11.1, 175, 35, 35) },
	{  0, performance,  VR_CFG_ALL_DOMAINS_ICC(11.1, 140, 35, 35) },
	{  0, baseline,     VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CML_S_P0P1_10_2) {
	{125, performance,  VR_CFG_ALL_DOMAINS_ICC(11.1, 245, 35, 35) },
	{125, baseline,     VR_CFG_ALL_DOMAINS_ICC(11.1, 210, 35, 35) },
	{ 80, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 210, 35, 35) },
	{ 65, performance,  VR_CFG_ALL_DOMAINS_ICC(11.1, 210, 35, 35) },
	{ 65, baseline,     VR_CFG_ALL_DOMAINS_ICC(11.1, 175, 35, 35) },
	{  0, performance,  VR_CFG_ALL_DOMAINS_ICC(11.1, 140, 35, 35) },
	{  0, baseline,     VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CML_S_G0G1_P0P1_6_2) {
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 140, 35, 35) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 104, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CML_S_G0G1_4) {
	{ 36, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 102, 35, 35) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 65, 35, 35) },
};
VR_CONFIG_ICC(PCI_DID_INTEL_CML_S_G0G1_2) {
	{ 36, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 60, 35, 35) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_ICC(11.1, 55, 35, 35) },
};

static const struct vr_lookup vr_config_icc[] = {
	VR_REFITEM_ICC(PCI_DID_INTEL_CNL_ID_U),
	VR_REFITEM_ICC(PCI_DID_INTEL_CNL_ID_Y),
	VR_REFITEM_ICC(PCI_DID_INTEL_WHL_ID_W_4),
	VR_REFITEM_ICC(PCI_DID_INTEL_WHL_ID_W_2),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_U),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_U_2),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_H_8),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_H),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_H_4),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_S_DT_2),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_S_DT_4),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_S_DT_8),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_S_S_8),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_S_WS_8),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_S),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_S_WS_6),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_S_WS_4),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_S_S_6),
	VR_REFITEM_ICC(PCI_DID_INTEL_CFL_ID_S_S_4),
	VR_REFITEM_ICC(PCI_DID_INTEL_CML_ULT),
	VR_REFITEM_ICC(PCI_DID_INTEL_CML_ULT_6_2),
	VR_REFITEM_ICC(PCI_DID_INTEL_CML_ULT_2_2),
	VR_REFITEM_ICC(PCI_DID_INTEL_CML_H_8_2),
	VR_REFITEM_ICC(PCI_DID_INTEL_CML_H),
	VR_REFITEM_ICC(PCI_DID_INTEL_CML_H_4_2),
	VR_REFITEM_ICC(PCI_DID_INTEL_CML_S_P0P1_8_2),
	VR_REFITEM_ICC(PCI_DID_INTEL_CML_S_P0P1_10_2),
	VR_REFITEM_ICC(PCI_DID_INTEL_CML_S_G0G1_P0P1_6_2),
	VR_REFITEM_ICC(PCI_DID_INTEL_CML_S_G0G1_4),
	VR_REFITEM_ICC(PCI_DID_INTEL_CML_S_G0G1_2),
};

VR_CONFIG_LL(PCI_DID_INTEL_CNL_ID_U) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.4, 2.0, 2.0) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CNL_ID_Y) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.4, 2.0, 2.0) },
};
VR_CONFIG_LL(PCI_DID_INTEL_WHL_ID_W_4) { /* unspecified */
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.8, 0, 0) },
};
VR_CONFIG_LL(PCI_DID_INTEL_WHL_ID_W_2) { /* unspecified */
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.4, 0, 0) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_U) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.4, 2.0, 2.0) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_U_2) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.4, 2.0, 2.0) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_H_8) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.8, 2.7, 2.7) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_H) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.8, 2.7, 2.7) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_H_4) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.8, 2.7, 2.7) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_S_WS_4) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.1, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_S_DT_4) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.1, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_S_DT_2) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.1, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_S_DT_8) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.6, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_S_S_8) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.6, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_S_WS_8) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.6, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_S) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.1, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_S_WS_6) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.1, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_S_S_6) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.1, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CFL_ID_S_S_4) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.1, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CML_ULT) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.8, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CML_ULT_6_2) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.8, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CML_ULT_2_2) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 2.4, 3.1, 3.1) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CML_H_8_2) {
	{ 0, value_not_set,  VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.1, 2.7, 2.7) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CML_H) {
	{ 0, value_not_set,  VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.1, 2.7, 2.7) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CML_H_4_2) {
	{ 0, value_not_set,  VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.1, 2.7, 2.7) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CML_S_P0P1_8_2) {
	{ 36, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.1, 4.0, 4.0) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.7, 4.0, 4.0) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CML_S_P0P1_10_2) {
	{ 36, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.1, 4.0, 4.0) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.7, 4.0, 4.0) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CML_S_G0G1_P0P1_6_2) {
	{125, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.1, 4.0, 4.0) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.7, 4.0, 4.0) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CML_S_G0G1_4) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.7, 4.0, 4.0) },
};
VR_CONFIG_LL(PCI_DID_INTEL_CML_S_G0G1_2) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_LOADLINE(10.3, 1.7, 4.0, 4.0) },
};


static const struct vr_lookup vr_config_ll[] = {
	VR_REFITEM_LL(PCI_DID_INTEL_CNL_ID_U),
	VR_REFITEM_LL(PCI_DID_INTEL_CNL_ID_Y),
	VR_REFITEM_LL(PCI_DID_INTEL_WHL_ID_W_4),
	VR_REFITEM_LL(PCI_DID_INTEL_WHL_ID_W_2),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_U),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_U_2),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_H_8),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_H),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_H_4),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_S_WS_4),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_S_DT_4),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_S_DT_2),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_S_DT_8),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_S_S_8),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_S_WS_8),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_S),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_S_WS_6),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_S_S_6),
	VR_REFITEM_LL(PCI_DID_INTEL_CFL_ID_S_S_4),
	VR_REFITEM_LL(PCI_DID_INTEL_CML_ULT),
	VR_REFITEM_LL(PCI_DID_INTEL_CML_ULT_6_2),
	VR_REFITEM_LL(PCI_DID_INTEL_CML_ULT_2_2),
	VR_REFITEM_LL(PCI_DID_INTEL_CML_H_8_2),
	VR_REFITEM_LL(PCI_DID_INTEL_CML_H),
	VR_REFITEM_LL(PCI_DID_INTEL_CML_H_4_2),
	VR_REFITEM_LL(PCI_DID_INTEL_CML_S_P0P1_8_2),
	VR_REFITEM_LL(PCI_DID_INTEL_CML_S_P0P1_10_2),
	VR_REFITEM_LL(PCI_DID_INTEL_CML_S_G0G1_P0P1_6_2),
	VR_REFITEM_LL(PCI_DID_INTEL_CML_S_G0G1_4),
	VR_REFITEM_LL(PCI_DID_INTEL_CML_S_G0G1_2),
};

VR_CONFIG_TDC(PCI_DID_INTEL_CFL_ID_H) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 80, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CFL_ID_S) {
	{ 95, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 100, 30, 30) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 91, 30, 30) },
	{ 35, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 74, 25, 25) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 74, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CFL_ID_S_DT_2) {
	{ 58, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 33, 30, 30) },
	{ 54, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 31, 30, 30) },
	{ 35, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 29, 25, 25) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 29, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CFL_ID_S_S_4) {
	{ 83, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 70, 30, 30) },
	{ 71, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 70, 30, 30) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 62, 30, 30) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 45, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CFL_ID_S_WS_4) {
	{ 83, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 70, 30, 30) },
	{ 71, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 70, 30, 30) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 62, 30, 30) },
	{ 35, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 45, 25, 25) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 45, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CFL_ID_S_DT_4) {
	{ 91, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 70, 30, 30) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 62, 30, 30) },
	{ 62, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 62, 30, 30) },
	{ 35, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 45, 25, 25) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 45, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CFL_ID_S_WS_6) {
	{ 95, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 100, 30, 30) },
	{ 80, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 89, 30, 30) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 91, 30, 30) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 74, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CFL_ID_S_S_6) {
	{ 95, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 100, 30, 30) },
	{ 80, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 89, 30, 30) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 91, 30, 30) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 74, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CFL_ID_S_S_8) {
	{ 95, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 150, 30, 30) },
	{ 80, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 150, 30, 30) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 146, 30, 30) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 74, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CFL_ID_S_WS_8) {
	{ 95, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 150, 30, 30) },
	{ 80, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 150, 30, 30) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 146, 30, 30) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 74, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CFL_ID_S_DT_8) {
	{ 127, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 150, 30, 30) },
	{ 95, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 150, 30, 30) },
	{ 80, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 150, 30, 30) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 146, 30, 30) },
	{ 35, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 74, 25, 25) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 74, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CML_ULT) {
	{  0, baseline,      VR_CFG_ALL_DOMAINS_TDC(4, 48, 22, 22) },
	{  0, performance,   VR_CFG_ALL_DOMAINS_TDC(4, 58, 22, 22) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CML_ULT_6_2) {
	{  0, baseline,      VR_CFG_ALL_DOMAINS_TDC(4, 48, 22, 22) },
	{  0, performance,   VR_CFG_ALL_DOMAINS_TDC(4, 58, 22, 22) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CML_ULT_2_2) {
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(4, 24, 22, 22) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CML_H_8_2) {
	{ 65, performance,   VR_CFG_ALL_DOMAINS_TDC(10, 146, 25, 25) },
	{ 65, baseline,      VR_CFG_ALL_DOMAINS_TDC(10, 117, 25, 25) },
	{  0, performance,   VR_CFG_ALL_DOMAINS_TDC(10, 125, 25, 25) },
	{  0, baseline,      VR_CFG_ALL_DOMAINS_TDC(10,  86, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CML_H) {
	{  0, performance,   VR_CFG_ALL_DOMAINS_TDC(10, 92, 25, 25) },
	{  0, baseline,      VR_CFG_ALL_DOMAINS_TDC(10, 80, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CML_H_4_2) {
	{  0, performance,   VR_CFG_ALL_DOMAINS_TDC(10, 80, 25, 25) },
	{  0, baseline,      VR_CFG_ALL_DOMAINS_TDC(10, 60, 25, 25) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CML_S_P0P1_8_2) {
	{ 36, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 175, 28, 28) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 100, 28, 28) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CML_S_P0P1_10_2) {
	{ 36, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 175, 28, 28) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 100, 28, 28) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CML_S_G0G1_P0P1_6_2) {
	{125, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 132, 28, 28) },
	{ 65, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 104, 28, 28) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 74, 28, 28) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CML_S_G0G1_4) {
	{ 36, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 68, 28, 28) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 42, 28, 28) },
};
VR_CONFIG_TDC(PCI_DID_INTEL_CML_S_G0G1_2) {
	{ 36, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 38, 28, 28) },
	{  0, value_not_set, VR_CFG_ALL_DOMAINS_TDC(10, 25, 28, 28) },
};

static const struct vr_lookup vr_config_tdc[] = {
	VR_REFITEM_TDC(PCI_DID_INTEL_CFL_ID_H),
	VR_REFITEM_TDC(PCI_DID_INTEL_CFL_ID_S),
	VR_REFITEM_TDC(PCI_DID_INTEL_CFL_ID_S_DT_2),
	VR_REFITEM_TDC(PCI_DID_INTEL_CFL_ID_S_S_4),
	VR_REFITEM_TDC(PCI_DID_INTEL_CFL_ID_S_WS_4),
	VR_REFITEM_TDC(PCI_DID_INTEL_CFL_ID_S_DT_4),
	VR_REFITEM_TDC(PCI_DID_INTEL_CFL_ID_S_WS_6),
	VR_REFITEM_TDC(PCI_DID_INTEL_CFL_ID_S_S_6),
	VR_REFITEM_TDC(PCI_DID_INTEL_CFL_ID_S_S_8),
	VR_REFITEM_TDC(PCI_DID_INTEL_CFL_ID_S_WS_8),
	VR_REFITEM_TDC(PCI_DID_INTEL_CFL_ID_S_DT_8),
	VR_REFITEM_TDC(PCI_DID_INTEL_CML_ULT),
	VR_REFITEM_TDC(PCI_DID_INTEL_CML_ULT_6_2),
	VR_REFITEM_TDC(PCI_DID_INTEL_CML_ULT_2_2),
	VR_REFITEM_TDC(PCI_DID_INTEL_CML_H_8_2),
	VR_REFITEM_TDC(PCI_DID_INTEL_CML_H),
	VR_REFITEM_TDC(PCI_DID_INTEL_CML_H_4_2),
	VR_REFITEM_TDC(PCI_DID_INTEL_CML_S_P0P1_8_2),
	VR_REFITEM_TDC(PCI_DID_INTEL_CML_S_P0P1_10_2),
	VR_REFITEM_TDC(PCI_DID_INTEL_CML_S_G0G1_P0P1_6_2),
	VR_REFITEM_TDC(PCI_DID_INTEL_CML_S_G0G1_4),
	VR_REFITEM_TDC(PCI_DID_INTEL_CML_S_G0G1_2),
};

static uint16_t get_sku_voltagelimit(int domain)
{
	return 1520;
}

static uint16_t get_sku_icc_max(const int domain,
				const uint16_t tdp,
				const uint16_t mch_id,
				const uint16_t igd_id)
{
	if (igd_id == 0xffff && ((domain == VR_GT_SLICED) || (domain == VR_GT_UNSLICED)))
		return 0;

	return load_table(vr_config_icc, ARRAY_SIZE(vr_config_icc), domain, tdp, mch_id);
}

void fill_vr_domain_config(void *params,
		int domain, const struct vr_config *chip_cfg)
{
	FSP_S_CONFIG *vr_params = (FSP_S_CONFIG *)params;
	const struct vr_config *cfg;
	static uint16_t mch_id = 0, igd_id = 0;
	const uint16_t tdp = cpu_get_power_max() / 1000;

	if (!mch_id) {
		struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
		mch_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;
	}
	if (!igd_id) {
		struct device *dev = pcidev_path_on_root(SA_DEVFN_IGD);
		igd_id = dev ? pci_read_config16(dev, PCI_DEVICE_ID) : 0xffff;
	}

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
	printk(BIOS_INFO, "VR config[%d]:\n", domain);
	printk(BIOS_INFO, "  Psi1Threshold:  %u\n", cfg->psi1threshold);
	printk(BIOS_INFO, "  Psi2Threshold:  %u\n", cfg->psi2threshold);
	printk(BIOS_INFO, "  Psi3Threshold:  %u\n", cfg->psi3threshold);
	printk(BIOS_INFO, "  Psi3Enable:     %u\n", cfg->psi3enable);
	printk(BIOS_INFO, "  Psi4Enable:     %u\n", cfg->psi4enable);
	printk(BIOS_INFO, "  ImonSlope:      %u\n", cfg->imon_slope);
	printk(BIOS_INFO, "  ImonOffset:     %u\n", cfg->imon_offset);

	/* If board provided non-zero value, use it. */
	if (cfg->voltage_limit)
		vr_params->VrVoltageLimit[domain] = cfg->voltage_limit;
	else
		vr_params->VrVoltageLimit[domain] = get_sku_voltagelimit(domain);
	printk(BIOS_INFO, "  VrVoltageLimit: %u\n", vr_params->VrVoltageLimit[domain]);

	if (cfg->icc_max)
		vr_params->IccMax[domain] = cfg->icc_max;
	else
		vr_params->IccMax[domain] = get_sku_icc_max(domain, tdp, mch_id, igd_id);
	printk(BIOS_INFO, "  IccMax:         %u\n", vr_params->IccMax[domain]);

	if (cfg->ac_loadline)
		vr_params->AcLoadline[domain] = cfg->ac_loadline;
	else
		vr_params->AcLoadline[domain] = load_table(vr_config_ll,
							   ARRAY_SIZE(vr_config_ll),
							   domain, tdp, mch_id);
	printk(BIOS_INFO, "  AcLoadline:     %u\n", vr_params->AcLoadline[domain]);

	if (cfg->dc_loadline)
		vr_params->DcLoadline[domain] = cfg->dc_loadline;
	else
		vr_params->DcLoadline[domain] = load_table(vr_config_ll,
							   ARRAY_SIZE(vr_config_ll),
							   domain, tdp, mch_id);
	printk(BIOS_INFO, "  DcLoadline:     %u\n", vr_params->DcLoadline[domain]);

	vr_params->TdcEnable[domain] = !cfg->tdc_disable;
	printk(BIOS_INFO, "  TdcEnable:      %u\n", vr_params->TdcEnable[domain]);

	if (cfg->tdc_powerlimit)
		vr_params->TdcPowerLimit[domain] = cfg->tdc_powerlimit;
	else
		vr_params->TdcPowerLimit[domain] = load_table(vr_config_tdc,
							      ARRAY_SIZE(vr_config_tdc),
							      domain, tdp, mch_id);
	printk(BIOS_INFO, "  TdcPowerLimit:  %u\n", vr_params->TdcPowerLimit[domain]);
}
