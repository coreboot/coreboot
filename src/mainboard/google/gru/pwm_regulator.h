/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __COREBOOT_SRC_MAINBOARD_GOOGLE_GRU_PWM_REGULATOR_H
#define __COREBOOT_SRC_MAINBOARD_GOOGLE_GRU_PWM_REGULATOR_H

enum pwm_regulator {
	PWM_REGULATOR_GPU = 0,
	PWM_REGULATOR_BIG,
	PWM_REGULATOR_LIT,
	PWM_REGULATOR_CENTERLOG
};

void pwm_regulator_configure(enum pwm_regulator pwm, int millivolt);

#endif /* ! __COREBOOT_SRC_MAINBOARD_GOOGLE_GRU_PWM_REGULATOR_H */
