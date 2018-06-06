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

#ifndef __SOC_MEDIATEK_MT8173_TIMER_H__
#define __SOC_MEDIATEK_MT8173_TIMER_H__

#include <soc/addressmap.h>
#include <types.h>

struct mt8173_gpt_regs {
	u32 irqen;
	u32 irqsta;
	u32 irqack;
	u32 reserved0;
	u32 gpt1_con;
	u32 gpt1_clk;
	u32 gpt1_cnt;
	u32 gpt1_compare;
	u32 gpt2_con;
	u32 gpt2_clk;
	u32 gpt2_cnt;
	u32 gpt2_compare;
	u32 gpt3_con;
	u32 gpt3_clk;
	u32 gpt3_cnt;
	u32 gpt3_compare;
	u32 gpt4_con;
	u32 gpt4_clk;
	u32 gpt4_cnt;
	u32 gpt4_compare;
	u32 gpt5_con;
	u32 gpt5_clk;
	u32 gpt5_cnt;
	u32 gpt5_compare;
	u32 gpt6_con;
	u32 gpt6_clk;
	u32 gpt6_cntl;
	u32 gpt6_comparel;
	u32 reserved1[2];
	u32 gpt6_cnth;
	u32 gpt6_compareh;
	u32 apxgpt_irqmask;
	u32 apxgpt_irqmask1;
};

static struct mt8173_gpt_regs *const mt8173_gpt = (void *)GPT_BASE;

enum {
	GPT_CON_EN = 0x01,
	GPT_CON_CLR = 0x02,
	GPT_MODE_ONE_SHOT = 0x00,
	GPT_MODE_REPEAT   = 0x10,
	GPT_MODE_KEEP_GO  = 0x20,
	GPT_MODE_FREERUN  = 0x30,
	GPT_SYS_CLK = 0x00,
	GPT_SYS_RTC = 0x01,
};

#endif	/* __SOC_MEDIATEK_MT8173_TIMER_H__ */
