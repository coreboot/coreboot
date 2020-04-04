/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <soc/infracfg.h>
#include <soc/smi.h>
#include <soc/mtcmos.h>

enum {
	DISP_PROT_STEP1_0_MASK	= 0x3 << 16,
	DISP_PROT_STEP2_0_MASK	= 0x3 << 10,
	DISP_PROT_STEP2_1_MASK	= 0xff,
};

void mtcmos_protect_display_bus(void)
{
	write32(&mt8183_infracfg->infra_topaxi_protecten_clr,
		DISP_PROT_STEP2_0_MASK);
	write32(&mt8183_smi->smi_common_clamp_en_clr, DISP_PROT_STEP2_1_MASK);
	write32(&mt8183_infracfg->infra_topaxi_protecten_1_clr,
		DISP_PROT_STEP1_0_MASK);
}
