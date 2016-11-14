/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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
#include <assert.h>
#include <boardid.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/grf.h>
#include <soc/pwm.h>

#include "pwm_regulator.h"

/*
 * Apparently a period of 3333 is determined by EEs to be ideal for our
 * board design / resistors / capacitors / regulators but due to
 * clock dividers we actually get 3337.
 */
#define PWM_PERIOD		3337
#define PWM_DESIGN_VOLTAGE_MIN	8000
#define PWM_DESIGN_VOLTAGE_MAX	15000

/* Later boards (Kevin rev6+, Gru rev2+) use different regulator ranges. */
int pwm_design_voltage_later[][2] = {
	[PWM_REGULATOR_GPU] = {7858, 12177},
	[PWM_REGULATOR_BIG] = {7987, 13022},
	[PWM_REGULATOR_LIT] = {7991, 13037},
	[PWM_REGULATOR_CENTERLOG] = {8001, 10497}
};

void pwm_regulator_configure(enum pwm_regulator pwm, int millivolt)
{
	int duty_ns, voltage_max, voltage_min;
	int voltage = millivolt * 10; /* for higer calculation accuracy */

	voltage_min = PWM_DESIGN_VOLTAGE_MIN;
	voltage_max = PWM_DESIGN_VOLTAGE_MAX;
	if (!(IS_ENABLED(CONFIG_BOARD_GOOGLE_KEVIN) && board_id() < 6) &&
	    !(IS_ENABLED(CONFIG_BOARD_GOOGLE_GRU) && board_id() < 2) &&
            !(IS_ENABLED(CONFIG_BOARD_GOOGLE_BOB) && board_id() < 2)) {
		voltage_min = pwm_design_voltage_later[pwm][0];
		voltage_max = pwm_design_voltage_later[pwm][1];
	}

	assert(voltage <= voltage_max && voltage >= voltage_min);

	/*
	 * Intentionally round down (higher volt) to be safe.
	 * eg, for the default min & max design voltage:
	 *    period = 3337, volt = 1.1: 1906
	 *    period = 3337, volt = 1.0: 2383
	 *    period = 3337, volt = 0.9: 2860
	 */
	duty_ns = PWM_PERIOD * (voltage_max - voltage)
			     / (voltage_max - voltage_min);

	pwm_init(pwm, PWM_PERIOD, duty_ns);

	switch (pwm) {
	case PWM_REGULATOR_GPU:
		gpio_input(GPIO(4, C, 2));	/* PWM0 remove pull-down */
		write32(&rk3399_grf->iomux_pwm_0, IOMUX_PWM_0);
		break;
	case PWM_REGULATOR_BIG:
		gpio_input(GPIO(4, C, 6));	/* PWM1 remove pull-down */
		write32(&rk3399_grf->iomux_pwm_1, IOMUX_PWM_1);
		break;
	case PWM_REGULATOR_LIT:
		gpio_input(GPIO(1, C, 3));	/* PWM2 remove pull-down */
		write32(&rk3399_pmugrf->iomux_pwm_2, IOMUX_PWM_2);
		break;
	case PWM_REGULATOR_CENTERLOG:
		gpio_input(GPIO(0, A, 6));	/* PWM3 remove pull-down */
		write32(&rk3399_pmugrf->iomux_pwm_3a, IOMUX_PWM_3_A);
		break;
	}
}
