/*
 * This file is part of the coreboot project.
 *
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
