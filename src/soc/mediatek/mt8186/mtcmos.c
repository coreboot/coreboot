/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/infracfg.h>
#include <soc/mtcmos.h>
#include <soc/spm.h>

enum {
	DISP_PROT_STEP_2_MASK		= 0x00000C06,
	DISP_PROT_STEP_1_MASK		= 0x00001800,
};

enum {
	TOP_AXI_PROT_EN_3_ADSP_TOP_STEP1	= 0x00001800,
	TOP_AXI_PROT_EN_3_ADSP_TOP_STEP2	= 0x00000003,
};

void mtcmos_adsp_power_on(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(adsp); i++)
		mtcmos_power_on(&adsp[i]);
}

void mtcmos_protect_display_bus(void)
{
	write32(&mt8186_infracfg_ao->infra_topaxi_protecten_clr,
		DISP_PROT_STEP_2_MASK);
	write32(&mt8186_infracfg_ao->infra_topaxi_protecten_1_clr,
		DISP_PROT_STEP_1_MASK);
}

void mtcmos_protect_audio_bus(void)
{
	/* No need to do protection since MT8186 doesn't have audio mtcmos. */
}

void mtcmos_protect_adsp_bus(void)
{
	write32(&mt8186_infracfg_ao->infra_topaxi_protecten_3_clr,
		TOP_AXI_PROT_EN_3_ADSP_TOP_STEP2);
	write32(&mt8186_infracfg_ao->infra_topaxi_protecten_3_clr,
		TOP_AXI_PROT_EN_3_ADSP_TOP_STEP1);
}
