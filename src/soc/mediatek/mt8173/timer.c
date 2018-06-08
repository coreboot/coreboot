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

#include <arch/io.h>
#include <soc/mcucfg.h>
#include <soc/timer.h>

void timer_prepare(void)
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
}
