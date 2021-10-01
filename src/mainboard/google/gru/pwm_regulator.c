/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
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
#define PWM_PERIOD			3337
#define PWM_DESIGN_VOLTAGE_MIN_OUTDATED	8000
#define PWM_DESIGN_VOLTAGE_MAX_OUTDATED	15000

/* Applies for Kevin rev6+ */
int kevin6_pwm_design_voltage[][2] = {
	[PWM_REGULATOR_GPU] = {7858, 12177},
	[PWM_REGULATOR_BIG] = {7987, 13022},
	[PWM_REGULATOR_LIT] = {7991, 13037},
	[PWM_REGULATOR_CENTERLOG] = {8001, 10497}
};

/* Applies for Gru rev2+, Bob, and Nefario. */
int pwm_design_voltage[][2] = {
	[PWM_REGULATOR_GPU] = {7864, 12177},
	[PWM_REGULATOR_BIG] = {8001, 13022},
	[PWM_REGULATOR_LIT] = {7977, 13078},
	[PWM_REGULATOR_CENTERLOG] = {7994, 10499}
};

/* Applies for Scarlet-based boards. */
int scarlet_pwm_design_voltage[][2] = {
	[PWM_REGULATOR_GPU] = {7996, 10990},
	[PWM_REGULATOR_BIG] = {8000, 12992},
	[PWM_REGULATOR_LIT] = {8021, 11996},
};

int pwm_enum_to_pwm_number[] = {
	[PWM_REGULATOR_GPU] = 0,
	[PWM_REGULATOR_LIT] = 2,
#if CONFIG(GRU_HAS_CENTERLOG_PWM)
	[PWM_REGULATOR_CENTERLOG] = 3,
#else
	[PWM_REGULATOR_CENTERLOG] = -1,
#endif
#if CONFIG(GRU_BASEBOARD_SCARLET)
	[PWM_REGULATOR_BIG] = 3,
#else
	[PWM_REGULATOR_BIG] = 1,
#endif
};

void pwm_regulator_configure(enum pwm_regulator pwm, int millivolt)
{
	int duty_ns, voltage_max, voltage_min;
	int voltage = millivolt * 10; /* for higher calculation accuracy */
	int pwm_number = pwm_enum_to_pwm_number[pwm];

	voltage_min = pwm_design_voltage[pwm][0];
	voltage_max = pwm_design_voltage[pwm][1];
	if ((CONFIG(BOARD_GOOGLE_KEVIN) && board_id() < 6) ||
	    (CONFIG(BOARD_GOOGLE_GRU) && board_id() < 2)) {
		voltage_min = PWM_DESIGN_VOLTAGE_MIN_OUTDATED;
		voltage_max = PWM_DESIGN_VOLTAGE_MAX_OUTDATED;
	} else if (CONFIG(BOARD_GOOGLE_KEVIN) && board_id() >= 6) {
		voltage_min = kevin6_pwm_design_voltage[pwm][0];
		voltage_max = kevin6_pwm_design_voltage[pwm][1];
	} else if (CONFIG(GRU_BASEBOARD_SCARLET)) {
		voltage_min = scarlet_pwm_design_voltage[pwm][0];
		voltage_max = scarlet_pwm_design_voltage[pwm][1];
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

	pwm_init(pwm_number, PWM_PERIOD, duty_ns);

	switch (pwm_number) {
	case 0:
		gpio_input(GPIO(4, C, 2));	/* PWM0 remove pull-down */
		write32(&rk3399_grf->iomux_pwm_0, IOMUX_PWM_0);
		break;
	case 1:
		gpio_input(GPIO(4, C, 6));	/* PWM1 remove pull-down */
		write32(&rk3399_grf->iomux_pwm_1, IOMUX_PWM_1);
		break;
	case 2:
		gpio_input(GPIO(1, C, 3));	/* PWM2 remove pull-down */
		write32(&rk3399_pmugrf->iomux_pwm_2, IOMUX_PWM_2);
		break;
	case 3:
		gpio_input(GPIO(0, A, 6));	/* PWM3 remove pull-down */
		write32(&rk3399_pmugrf->iomux_pwm_3a, IOMUX_PWM_3_A);
		break;
	default:
		die("incorrect board configuration");
	}
}
