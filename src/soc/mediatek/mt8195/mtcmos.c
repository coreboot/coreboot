/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/infracfg.h>
#include <soc/mtcmos.h>

enum {
	VPPSYS0_PROT_STEP_6_MASK	= 0x00100000,
	VPPSYS0_PROT_STEP_5_MASK	= 0x0007F8FF,
	VPPSYS0_PROT_STEP_4_MASK	= 0x00800000,
	VPPSYS0_PROT_STEP_3_MASK	= 0x01600300,
	VPPSYS0_PROT_STEP_2_MASK	= 0x80381DC7,
	VPPSYS0_PROT_STEP_1_MASK	= 0x00000400,

	VDOSYS0_PROT_STEP_5_MASK	= 0x00200000,
	VDOSYS0_PROT_STEP_4_MASK	= 0x3FC00000,
	VDOSYS0_PROT_STEP_3_MASK	= 0x00000040,
	VDOSYS0_PROT_STEP_2_MASK	= 0x00800000,
	VDOSYS0_PROT_STEP_1_MASK	= 0x403E6238,

	VPPSYS1_PROT_STEP_3_MASK	= 0x000400C0,
	VPPSYS1_PROT_STEP_2_MASK	= 0x00800000,
	VPPSYS1_PROT_STEP_1_MASK	= 0x000001E0,

	VDOSYS1_PROT_STEP_3_MASK	= 0x00000400,
	VDOSYS1_PROT_STEP_2_MASK	= 0x00400000,
	VDOSYS1_PROT_STEP_1_MASK	= 0xC0000000,

	ADSP_PROT_STEP_1_MASK		= 0x0001D000,

	AUDIO_PROT_STEP_1_MASK		= 0x00000A00,
};

void mtcmos_protect_display_bus(void)
{
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_sub_infra_vdnr_clr,
		VPPSYS0_PROT_STEP_6_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_mm_clr_2,
		VPPSYS0_PROT_STEP_5_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_clr,
		VPPSYS0_PROT_STEP_4_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_mm_clr_2,
		VPPSYS0_PROT_STEP_3_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VPPSYS0_PROT_STEP_2_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_clr,
		VPPSYS0_PROT_STEP_1_MASK);

	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_sub_infra_vdnr_clr,
		VDOSYS0_PROT_STEP_5_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VDOSYS0_PROT_STEP_4_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_clr,
		VDOSYS0_PROT_STEP_3_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_mm_clr_2,
		VDOSYS0_PROT_STEP_2_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VDOSYS0_PROT_STEP_1_MASK);

	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_mm_clr_2,
		VPPSYS1_PROT_STEP_3_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VPPSYS1_PROT_STEP_2_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VPPSYS1_PROT_STEP_1_MASK);

	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_mm_clr_2,
		VDOSYS1_PROT_STEP_3_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VDOSYS1_PROT_STEP_2_MASK);
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_mm_clr,
		VDOSYS1_PROT_STEP_1_MASK);
}

void mtcmos_protect_audio_bus(void)
{
	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_clr_2,
		ADSP_PROT_STEP_1_MASK);

	write32(&mt8195_infracfg_ao->infra_topaxi_protecten_clr_2,
		AUDIO_PROT_STEP_1_MASK);
}
