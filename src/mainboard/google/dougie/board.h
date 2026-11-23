/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _COREBOOT_SRC_MAINBOARD_GOOGLE_DOUGIE_BOARD_H_
#define _COREBOOT_SRC_MAINBOARD_GOOGLE_DOUGIE_BOARD_H_

#include <gpio.h>

#define GPIO_EC_AP_INT		GPIO(18)
#define GPIO_GSC_AP_INT		GPIO(104)

/* Display specific gpios */
#define GPIO_PANEL_POWER_ON	GPIO(80)
#define GPIO_PANEL_HPD		GPIO(60)

/* Backlight: PMIC (SPMI SID 2) GPIO6 regulator enable, GPIO7 enable, GPIO8 PWM */
#define BACKLIGHT_PMIC_SID	2
#define BACKLIGHT_PMIC_REG_EN	6
#define BACKLIGHT_PMIC_EN	7
#define BACKLIGHT_PMIC_PWM	8

/* Bottom margin (pixels) of the firmware splash footer logo */
#define SPLASH_LOGO_BOTTOM_MARGIN	200

void setup_chromeos_gpios(void);

#endif /* _COREBOOT_SRC_MAINBOARD_GOOGLE_DOUGIE_BOARD_H_ */
