/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_TIMER_V2_H
#define SOC_MEDIATEK_COMMON_TIMER_V2_H

#include <device/mmio.h>
#include <soc/timer_common.h>
#include <types.h>

#define GPT_MHZ	13

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

#endif
