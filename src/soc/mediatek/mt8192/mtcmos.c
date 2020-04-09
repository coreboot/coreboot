/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/infracfg.h>
#include <soc/mtcmos.h>

enum {
	DISP_PROT_STEP1_0_MASK	= 0x05015405,
	DISP_PROT_STEP1_1_MASK	= 0x00001100,
	DISP_PROT_STEP2_0_MASK	= 0x00800040,
	DISP_PROT_STEP2_1_MASK	= 0x0a02800a,
	DISP_PROT_STEP2_2_MASK	= 0x00002200,

	AUDIO_PROT_STEP1_0_MASK	= 0x00000010,
};

void mtcmos_protect_display_bus(void)
{
	write32(&mt8192_infracfg->infra_topaxi_protecten_clr,
		DISP_PROT_STEP2_0_MASK);
	write32(&mt8192_infracfg->infra_topaxi_protecten_mm_clr,
		DISP_PROT_STEP2_1_MASK);
	write32(&mt8192_infracfg->infra_topaxi_protecten_mm_clr_2,
		DISP_PROT_STEP2_2_MASK);
	write32(&mt8192_infracfg->infra_topaxi_protecten_mm_clr,
		DISP_PROT_STEP1_0_MASK);
	write32(&mt8192_infracfg->infra_topaxi_protecten_mm_clr_2,
		DISP_PROT_STEP1_1_MASK);
}

void mtcmos_protect_audio_bus(void)
{
	write32(&mt8192_infracfg->infra_topaxi_protecten_clr_2,
		AUDIO_PROT_STEP1_0_MASK);
}
