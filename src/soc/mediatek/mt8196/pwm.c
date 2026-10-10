/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <device/mmio.h>
#include <soc/pwm.h>
#include <timer.h>
#include <types.h>

void mtk_disp_pwm_set_en(bool en)
{
	SET32_BITFIELDS(&mtk_disp_pwm->en, DISP_PWM_EN, en);
}

int mtk_disp_pwm_config(u32 period_ns, u32 level, u32 max)
{
	u64 cycles, div, duty_ns;
	u32 clk_div, period, high_width;

	cycles = (u64)period_ns * DISP_PWM_CLK_HZ / NSECS_PER_SEC;
	clk_div = cycles >> DISP_PWM_PERIOD_BITS;

	if (!period_ns || !max || clk_div > DISP_PWM_CLKDIV_MAX) {
		printk(BIOS_ERR, "%s: invalid period %u ns / max %u / clk_div %u\n",
		       __func__, period_ns, max, clk_div);
		return -1;
	}

	/* pwm_bl: duty = period * levels[brightness] / scale */
	duty_ns = (u64)period_ns * MIN(level, max) / max;

	/* pwm-mtk-disp: mtk_disp_pwm_apply() -- clk_div, PERIOD = cycles - 1 */
	div = (u64)NSECS_PER_SEC * (clk_div + 1);
	period = (u64)period_ns * DISP_PWM_CLK_HZ / div;
	if (period)
		period--;
	high_width = duty_ns * DISP_PWM_CLK_HZ / div;

	/* Bypass double-buffer so writes take effect immediately. */
	SET32_BITFIELDS(&mtk_disp_pwm->bls_debug, DISP_PWM_BLS_DEBUG_BYPASS, 0x3);
	SET32_BITFIELDS(&mtk_disp_pwm->con0, DISP_PWM_CON0_CLKDIV, clk_div);
	WRITE32_BITFIELDS(&mtk_disp_pwm->con1,
			  DISP_PWM_CON1_PERIOD, period,
			  DISP_PWM_CON1_HIGH_WIDTH, high_width);

	return 0;
}
