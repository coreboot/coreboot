/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/grf.h>
#include <soc/soc.h>
#include <soc/pwm.h>
#include <soc/clock.h>
#include <timer.h>

struct pwm_ctl {
	u32	pwm_cnt;
	u32	pwm_period_hpr;
	u32	pwm_duty_lpr;
	u32	pwm_ctrl;
};

struct rk_pwm_regs {
	struct pwm_ctl pwm[4];
	u32	intsts;
	u32	int_en;
};
check_member(rk_pwm_regs, int_en, 0x44);

#define RK_PWM_DISABLE                  (0 << 0)
#define RK_PWM_ENABLE                   (1 << 0)

#define PWM_ONE_SHOT                    (0 << 1)
#define PWM_CONTINUOUS                  (1 << 1)
#define RK_PWM_CAPTURE                  (1 << 2)

#define PWM_DUTY_POSTIVE                (1 << 3)
#define PWM_DUTY_NEGATIVE               (0 << 3)

#define PWM_INACTIVE_POSTIVE            (1 << 4)
#define PWM_INACTIVE_NEGATIVE           (0 << 4)

#define PWM_OUTPUT_LEFT                 (0 << 5)
#define PWM_OUTPUT_CENTER               (1 << 5)

#define PWM_LP_ENABLE                   (1 << 8)
#define PWM_LP_DISABLE                  (0 << 8)

#define PWM_SEL_SCALE_CLK			(1 << 9)
#define PWM_SEL_SRC_CLK				(0 << 9)

struct rk_pwm_regs *rk_pwm = (void *)RK_PWM_BASE;

void pwm_init(u32 id, u32 period_ns, u32 duty_ns)
{
	unsigned long period, duty;

#if CONFIG(SOC_ROCKCHIP_RK3288)
	/*use rk pwm*/
	write32(&rk3288_grf->soc_con2, RK_SETBITS(1 << 0));
#endif

	write32(&rk_pwm->pwm[id].pwm_ctrl, PWM_SEL_SRC_CLK |
		PWM_OUTPUT_LEFT | PWM_LP_DISABLE | PWM_CONTINUOUS |
		PWM_DUTY_POSTIVE | PWM_INACTIVE_POSTIVE | RK_PWM_DISABLE);

	period = (PWM_CLOCK_HZ / 1000) * period_ns / USECS_PER_SEC;
	duty = (PWM_CLOCK_HZ / 1000) * duty_ns / USECS_PER_SEC;

	write32(&rk_pwm->pwm[id].pwm_period_hpr, period);
	write32(&rk_pwm->pwm[id].pwm_duty_lpr, duty);
	setbits32(&rk_pwm->pwm[id].pwm_ctrl, RK_PWM_ENABLE);
}
