/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
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
	write32(&mt8173_mcucfg->xgpt_ctl, (0x1 | ((26 / TIMER_MHZ) << 8)));
}
