/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __SOC_MEDIATEK_MT8173_INFRACFG_H__
#define __SOC_MEDIATEK_MT8173_INFRACFG_H__

#include <soc/addressmap.h>
#include <types.h>

struct mt8173_infracfg_regs {
	u32 top_ckmuxsel;
	u32 reserved0;
	u32 top_ckdiv1;
	u32 reserved1;
	u32 top_dcmctl;
	u32 top_dcmdbc;
	u32 reserved2[6];
	u32 infra_rst0;
	u32 infra_rst1;
	u32 reserved3[2];
	u32 infra_pdn0;
	u32 infra_pdn1;
	u32 reserved4[118];
	u32 topaxi_prot_en;
	u32 topaxi_prot_sta0;
	u32 topaxi_prot_sta1;   /* 0x228 */
};

check_member(mt8173_infracfg_regs, infra_pdn0, 0x40);
check_member(mt8173_infracfg_regs, topaxi_prot_sta1, 0x228);

static struct mt8173_infracfg_regs *const mt8173_infracfg =
	(void *)INFRACFG_AO_BASE;

enum {
	INFRA_PMIC_WRAP_RST = 1 << 7,
	L2C_SRAM_PDN = 1 << 7
};

#endif	/* __SOC_MEDIATEK_MT8173_INFRACFG_H__ */
