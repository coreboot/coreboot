/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2014-2015, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef _TEGRA210_CLST_CLK_H_
#define _TEGRA210_CLST_CLK_H_

#include <compiler.h>

/* Cluster Clock (CLUSTER_CLOCKS_PUBLIC_) regs */
struct  __packed clst_clk_ctlr {
	u32 pllx_base;			/* _PLLX_BASE,              0x000 */
	u32 pllx_misc;			/* _PLLX_MISC,              0x004 */
	u32 pllx_misc1;			/* _PLLX_MISC_1,            0x008 */
	u32 pllx_misc2;			/* _PLLX_MISC_2,            0x00c */
	u32 pllx_misc3;			/* _PLLX_MISC_3,            0x010 */
	u32 pllx_hw_ctrl_cfg;		/* _PLLX_HW_CTRL_CFG,       0x014 */
	u32 pllx_sw_ramp_cfg;		/* _PLLX_SW_RAMP_CFG,       0x018 */
	u32 pllx_hw_ctrl_status;	/* _PLLX_HW_CTRL_STATUS,    0x01c */
	u32 cclk_brst_pol;		/* _CCLK_BURST_POLICY,      0x020 */
	u32 super_cclk_div;		/* _SUPER_CCLK_DIVIDER,     0x024 */
	u32 _rsv1[10];			/*                      0x028-04c */
	u32 shaper;			/* _SHAPER,                 0x050 */
	u32 shaper1;			/* _SHAPER_1,               0x054 */
	u32 _rsv2[80];			/*                      0x058-194 */
	u32 misc_ctrl;			/* _MISC_CTRL,              0x198 */
};
check_member(clst_clk_ctlr, misc_ctrl, 0x198);

/* CC_CCLK_BRST_POL */
enum {
	CC_CCLK_BRST_POL_PLLX_OUT0_LJ = 0x8,
};

/* CC_SUPER_CCLK_DIVIDER */
enum {
	CC_SUPER_CCLK_DIVIDER_SUPER_CDIV_ENB = 1 << 31
};

/* PLLX_MISC3 */
enum {
	PLLX_IDDQ = 1 << 3,
};

/* MISC_CTRL */
enum {
	CLK_SWITCH_MATCH = 1 << 5,
};

#define CLK_SWITCH_TIMEOUT_US	1000
#endif	/* _TEGRA210_CLST_CLK_H_ */
