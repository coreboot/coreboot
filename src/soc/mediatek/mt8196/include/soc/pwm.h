/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8196_PWM_H
#define SOC_MEDIATEK_MT8196_PWM_H

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <types.h>

struct disp_pwm_regs {
	u32 en;
	u32 reserved_0x04[5];
	u32 con0;
	u32 con1;
	u32 reserved_0x20[24];
	u32 bls_debug;
};

check_member(disp_pwm_regs, con0, 0x18);
check_member(disp_pwm_regs, con1, 0x1c);
check_member(disp_pwm_regs, bls_debug, 0x80);

static struct disp_pwm_regs *const mtk_disp_pwm = (void *)DISP_PWM_BASE;

#define DISP_PWM_CLK_HZ			(130 * MHz)	/* CLK_CK_DISP_PWM_SEL = ck_osc_d4 */
#define DISP_PWM_PERIOD_BITS		12
#define DISP_PWM_CLKDIV_MAX		0x3ff

DEFINE_BIT(DISP_PWM_EN, 0)
DEFINE_BITFIELD(DISP_PWM_CON0_CLKDIV, 25, 16)
DEFINE_BITFIELD(DISP_PWM_CON1_PERIOD, 15, 0)
DEFINE_BITFIELD(DISP_PWM_CON1_HIGH_WIDTH, 31, 16)
DEFINE_BITFIELD(DISP_PWM_BLS_DEBUG_BYPASS, 1, 0)

/*
 * Set or clear the EN bit only. The kernel's pwm-mtk-disp driver reads EN
 * at probe and otherwise assumes DISP_PWM is off, turning the backlight off
 * during init.
 */
void mtk_disp_pwm_set_en(bool en);

/*
 * Program CLKDIV/PERIOD/HIGH_WIDTH for period_ns at level/max duty (mirrors
 * the kernel's pwm_bl + pwm-mtk-disp computation step by step, so the result
 * is bit-exact by construction instead of via board-specific constants).
 * Does not touch EN.
 *
 * @level/@max: brightness fraction; max must be non-zero.
 * Returns 0 on success, -1 if period_ns or max is invalid.
 */
int mtk_disp_pwm_config(u32 period_ns, u32 level, u32 max);

#endif /* SOC_MEDIATEK_MT8196_PWM_H */
