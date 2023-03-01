/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/infracfg.h>
#include <soc/mtcmos.h>
#include <soc/spm.h>

enum {
	VPPSYS0_PROT_STEP_6_MASK	= 0x00100000,
	VPPSYS0_PROT_STEP_5_MASK	= 0x00276C16,
	VPPSYS0_PROT_STEP_4_MASK	= 0x00800000,
	VPPSYS0_PROT_STEP_3_MASK	= 0x00800200,
	VPPSYS0_PROT_STEP_2_MASK	= 0x80302846,
	VPPSYS0_PROT_STEP_1_MASK	= 0x00000400,
	VDOSYS0_PROT_STEP_4_MASK	= 0x00000060,
	VDOSYS0_PROT_STEP_3_MASK	= 0x00800000,
	VDOSYS0_PROT_STEP_2_MASK	= 0x00000020,
	VDOSYS0_PROT_STEP_1_MASK	= 0x00100000,
	VPPSYS1_PROT_STEP_3_MASK	= 0x00040000,
	VPPSYS1_PROT_STEP_2_MASK	= 0x00800000,
	VPPSYS1_PROT_STEP_1_MASK	= 0x00000020,
	VDOSYS1_PROT_STEP_3_MASK	= 0x00000400,
	VDOSYS1_PROT_STEP_2_MASK	= 0x00400000,
	VDOSYS1_PROT_STEP_1_MASK	= 0x40000000,
	ADSP_PROT_STEP_2_MASK		= 0x00001000,
	ADSP_PROT_STEP_1_MASK		= 0x00100000,
	AUDIO_PROT_STEP_2_MASK		= 0x00600000,
	AUDIO_PROT_STEP_1_MASK		= 0x00000F00,
};

void mtcmos_set_scpd_ext_buck_iso(const struct power_domain_data *pd)
{
	clrbits32(&mtk_spm->ext_buck_iso, pd->ext_buck_iso_bits);
}

void mtcmos_protect_display_bus(void)
{
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_sub_infra_vdnr_clr,
		VPPSYS0_PROT_STEP_6_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_mm_clr_2,
		VPPSYS0_PROT_STEP_5_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_clr,
		VPPSYS0_PROT_STEP_4_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_mm_clr_2,
		VPPSYS0_PROT_STEP_3_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VPPSYS0_PROT_STEP_2_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_clr,
		VPPSYS0_PROT_STEP_1_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_sub_infra_vdnr_clr,
		VDOSYS0_PROT_STEP_4_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VDOSYS0_PROT_STEP_3_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_clr,
		VDOSYS0_PROT_STEP_2_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VDOSYS0_PROT_STEP_1_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_mm_clr_2,
		VPPSYS1_PROT_STEP_3_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VPPSYS1_PROT_STEP_2_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VPPSYS1_PROT_STEP_1_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_mm_clr_2,
		VDOSYS1_PROT_STEP_3_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VDOSYS1_PROT_STEP_2_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VDOSYS1_PROT_STEP_1_MASK);
}

void mtcmos_protect_audio_bus(void)
{
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_clr_2,
		ADSP_PROT_STEP_2_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_clr_2,
		ADSP_PROT_STEP_1_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_clr_2,
		AUDIO_PROT_STEP_2_MASK);
	write32(&mt8188_infracfg_ao->infra_topaxi_protecten_clr_2,
		AUDIO_PROT_STEP_1_MASK);
}
