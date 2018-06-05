/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
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
