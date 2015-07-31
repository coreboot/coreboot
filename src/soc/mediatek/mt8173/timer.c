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

#include <arch/io.h>
#include <console/console.h>
#include <timer.h>
#include <delay.h>
#include <thread.h>

#include <soc/addressmap.h>
#include <soc/mcucfg.h>
#include <soc/timer.h>

#define GPT4_MHZ	13

void timer_monotonic_get(struct mono_time *mt)
{
        mono_time_set_usecs(mt, read32(&mt8173_gpt->gpt4_cnt) / GPT4_MHZ);
}

/**
 * init_timer - initialize timer
 */
void init_timer(void)
{
	/* Set XGPT_IDX to 0, then the bit field of XGPT_CTL will be programmed
	 * with following definition.
	 *
	 * [10: 8] Clock mode
	 *         100: 26Mhz / 4
	 *         010: 26Mhz / 2
	 *         001: 26Mhz
	 * [ 1: 1] Halt-on-debug enable bit
	 * [ 0: 0] XGPT enable bit
	 */
	write32(&mt8173_mcucfg->xgpt_idx, 0);
	/* Set clock mode to 13Mhz and enable XGPT */
	write32(&mt8173_mcucfg->xgpt_ctl, (0x1 | ((26 / GPT4_MHZ) << 8)));

	/* Disable GPT4 and clear the counter */
	clrbits_le32(&mt8173_gpt->gpt4_con, GPT_CON_EN);
	setbits_le32(&mt8173_gpt->gpt4_con, GPT_CON_CLR);

	/* Set clock source to system clock and set clock divider to 1 */
	write32(&mt8173_gpt->gpt4_clk, GPT_SYS_CLK | 0x0);
	/* Set operation mode to FREERUN mode and enable GTP4 */
	write32(&mt8173_gpt->gpt4_con, GPT_CON_EN | GPT_MODE_FREERUN);
}

