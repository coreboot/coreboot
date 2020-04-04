/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <timer.h>
#include <delay.h>
#include <thread.h>

#include <soc/addressmap.h>
#include <soc/timer.h>

static struct mtk_gpt_regs *const mtk_gpt = (void *)GPT_BASE;

__weak void timer_prepare(void) { /* do nothing */ }

static uint64_t timer_raw_value(void)
{
	/*
	 * According to "General-Purpose Timer (GPT).pdf", The read operation of
	 * gpt6_cnt_l will make gpt6_cnt_h fixed until the next read operation
	 * of gpt6_cnt_l. Therefore, we must read gpt6_cnt_l before gpt6_cnt_h.
	 */
	uint32_t low = read32(&mtk_gpt->gpt6_cnt_l);
	uint32_t high = read32(&mtk_gpt->gpt6_cnt_h);

	return low | (uint64_t)high << 32;
}

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, timer_raw_value() / GPT_MHZ);
}

void init_timer(void)
{
	timer_prepare();

	/* Disable timer and clear the counter */
	clrbits32(&mtk_gpt->gpt6_con, GPT_CON_EN);
	setbits32(&mtk_gpt->gpt6_con, GPT_CON_CLR);

	/* Set clock source to system clock and set clock divider to 1 */
	write32(&mtk_gpt->gpt6_clk, GPT_SYS_CLK | GPT_CLK_DIV1);
	/* Set operation mode to FREERUN mode and enable timer */
	write32(&mtk_gpt->gpt6_con, GPT_CON_EN | GPT_MODE_FREERUN);
}
