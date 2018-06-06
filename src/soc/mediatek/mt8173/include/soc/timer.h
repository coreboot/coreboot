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
 */

#ifndef SOC_MEDIATEK_COMMON_TIMER_H
#define SOC_MEDIATEK_COMMON_TIMER_H

#include <soc/addressmap.h>
#include <types.h>

#define GPT4_MHZ	13

struct mtk_gpt_regs {
	u32 reserved[16];
	u32 gpt4_con;
	u32 gpt4_clk;
	u32 gpt4_cnt;
};

check_member(mtk_gpt_regs, gpt4_con, 0x0040);
check_member(mtk_gpt_regs, gpt4_clk, 0x0044);
check_member(mtk_gpt_regs, gpt4_cnt, 0x0048);

enum {
	GPT_CON_EN        = 0x01,
	GPT_CON_CLR       = 0x02,
	GPT_MODE_FREERUN  = 0x30,
	GPT_SYS_CLK       = 0x00,
	GPT_CLK_DIV1      = 0x00,
};

/*
 * This is defined as weak no-ops that can be overridden by legacy SOCs. Some
 * legacy SOCs need specific settings before init timer. And we expect future
 * SOCs will not need it.
 */
void timer_prepare(void);

#endif
