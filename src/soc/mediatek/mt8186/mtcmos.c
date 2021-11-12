/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/infracfg.h>
#include <soc/mtcmos.h>

enum {
	DISP_PROT_STEP_2_MASK		= 0x00000C06,
	DISP_PROT_STEP_1_MASK		= 0x00001800,
};

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
