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
#include <compiler.h>
#include <console/console.h>
#include <timer.h>
#include <delay.h>
#include <thread.h>

#include <soc/addressmap.h>
#include <soc/timer.h>

static struct mtk_gpt_regs *const mtk_gpt = (void *)GPT_BASE;

__weak void timer_prepare(void) { /* do nothing */ }

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, read32(&mtk_gpt->gpt4_cnt) / GPT4_MHZ);
}

void init_timer(void)
{
	timer_prepare();

	/* Disable GPT4 and clear the counter */
	clrbits_le32(&mtk_gpt->gpt4_con, GPT_CON_EN);
	setbits_le32(&mtk_gpt->gpt4_con, GPT_CON_CLR);

	/* Set clock source to system clock and set clock divider to 1 */
	write32(&mtk_gpt->gpt4_clk, GPT_SYS_CLK | GPT_CLK_DIV1);
	/* Set operation mode to FREERUN mode and enable GTP4 */
	write32(&mtk_gpt->gpt4_con, GPT_CON_EN | GPT_MODE_FREERUN);
}
