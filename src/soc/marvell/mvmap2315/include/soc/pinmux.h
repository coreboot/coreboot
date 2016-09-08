/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#ifndef __SOC_MARVELL_MVMAP2315_PINMUX_H__
#define __SOC_MARVELL_MVMAP2315_PINMUX_H__

#include <stdint.h>

#include <soc/addressmap.h>
#include <types.h>

#define PINMUX(a, b, c, d, e, f) ((struct mvmap2315_pinmux) {		\
				     .pad = a,				\
				     .fun_sel = b,			\
				     .raw_sel = c,			\
				     .dgtb_sel = d,			\
				     .slew = e,				\
				     .pull_sel = f})

#define MVMAP2315_PADWRAP_FUNC_SEL	(BIT(0) | BIT(1) | BIT(2))
#define MVMAP2315_PADWRAP_RAW_SEL	BIT(3)
#define MVMAP2315_PADWRAP_DGTB_SEL	(BIT(4) | BIT(5) | BIT(6) | BIT(7))
#define MVMAP2315_PADWRAP_SLEW		(BIT(11) | BIT(12))
#define MVMAP2315_PADWRAP_PD_EN		BIT(13)
#define MVMAP2315_PADWRAP_PU_EN		BIT(14)

#define MVMAP2315_PADWRAP_FUNC_SEL_SHIFT	0
#define MVMAP2315_PADWRAP_RAW_SEL_SHIFT		3
#define MVMAP2315_PADWRAP_DGTB_SEL_SHIFT	4
#define MVMAP2315_PADWRAP_SLEW_SHIFT		11
struct mvmap2315_pinmux_regs {
	 u32 io_pad_piocfg[72];
	 u8 _reserved0[0xee0];
	 u32 pc_pwrdwn_g29_pwrdn;
	 u32 pc_v18en_lvl_g29;
	 u32 vdd3p3_1p8_g29_reg_pwrdn;
	 u32 pc_pwrdwn_g30_pwrdn;
	 u32 pc_v18en_lvl_g30;
	 u32 vdd3p3_1p8_g30_reg_pwrdn;
	 u32 pc_pwrdwn_g31_pwrdn;
	 u32 pc_v18en_lvl_g31;
	 u32 vdd3p3_1p8_g31_reg_pwrdn;
	 u32 pc_pwrdwn_g32_pwrdn;
	 u32 pc_v18en_lvl_g32;
	 u32 vdd3p3_1p8_g32_reg_pwrdn;
	 u32 pc_pwrdwn_g33_pwrdn;
	 u32 pc_v18en_lvl_g33;
	 u32 vdd3p3_1p8_g33_reg_pwrdn;
};

check_member(mvmap2315_pinmux_regs, vdd3p3_1p8_g33_reg_pwrdn, 0x1038);
static struct mvmap2315_pinmux_regs * const mvmap2315_pinmux
					= (void *)MVMAP2315_PINMUX_BASE;

struct mvmap2315_pinmux {
	u32 pad;
	u32 fun_sel;
	u32 raw_sel;
	u32 dgtb_sel;
	u32 slew;
	u32 pull_sel;
};

check_member(mvmap2315_pinmux, pull_sel, 0x14);

enum {
	PULLNONE = 0,
	PULLDOWN = 1,
	PULLUP = 2
};

void set_pinmux(struct mvmap2315_pinmux pinmux);

#endif /* __SOC_MARVELL_MVMAP2315_PINMUX_H__ */
