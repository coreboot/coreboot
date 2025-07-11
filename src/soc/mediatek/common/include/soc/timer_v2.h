/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_TIMER_V2_H
#define SOC_MEDIATEK_COMMON_TIMER_V2_H

#include <device/mmio.h>
#include <soc/timer_common.h>
#include <types.h>

#define TIMER_MHZ	13

enum {
	TIE_0_EN = 1 << 3,
	COMP_15_EN = 1 << 10,
	COMP_20_EN = 1 << 11,
	COMP_25_EN = 1 << 12,

	COMP_FEATURE_MASK = COMP_15_EN | COMP_20_EN | COMP_25_EN | TIE_0_EN,

	COMP_15_MASK = COMP_15_EN,
	COMP_20_MASK = COMP_20_EN | TIE_0_EN,
	COMP_25_MASK = COMP_20_EN | COMP_25_EN,
};

struct mtk_gpt_regs {
	u32 reserved1[40];
	u32 gpt6_con;
	u32 reserved2;
	u32 gpt6_cnt_l;
	u32 reserved3;
	u32 gpt6_cnt_h;
};

check_member(mtk_gpt_regs, gpt6_con, 0x00A0);
check_member(mtk_gpt_regs, gpt6_cnt_l, 0x00A8);
check_member(mtk_gpt_regs, gpt6_cnt_h, 0x00B0);

DEFINE_BIT(GPT6_CON_EN6, 0)
DEFINE_BIT(GPT6_CON_CLR6, 1)
DEFINE_BITFIELD(GPT6_CON_MODE6, 6, 5)

#define GPT6_CLOCK_REG(x) x->gpt6_con
DEFINE_BITFIELD(GPT6_CLK_CLKDIV6, 3, 0)
DEFINE_BITFIELD(GPT6_CLK_CLK6, 13, 10)

void timer_set_compensation(void);

#endif
