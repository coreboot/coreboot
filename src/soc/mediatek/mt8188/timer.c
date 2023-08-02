/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/lib_helpers.h>
#include <commonlib/helpers.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/timer.h>

static struct mtk_gpt_regs *const mtk_gpt = (void *)GPT_BASE;

void init_timer(void)
{
	timer_prepare();

	raw_write_cntfrq_el0(13 * MHz);

	/* TODO: remove GPT timer init after DRAM blob switching to arch timer */
	/* Disable timer and clear the counter */
	clrbits32(&mtk_gpt->gpt6_con, GPT6_CON_EN);
	setbits32(&mtk_gpt->gpt6_con, GPT6_CON_CLR);

	/* Set clock source to system clock and set clock divider to 1 */
	SET32_BITFIELDS(&GPT6_CLOCK_REG(mtk_gpt),
			GPT6_CLK_CLK6, GPT6_CLK_CLK6_SYS,
			GPT6_CLK_CLKDIV6, GPT6_CLK_CLKDIV_DIV1);
	/* Set operation mode to FREERUN mode and enable timer */
	SET32_BITFIELDS(&mtk_gpt->gpt6_con,
			GPT6_CON_MODE6, GPT6_MODE_FREERUN,
			GPT6_CON_EN6, GPT6_CON_EN);
}
