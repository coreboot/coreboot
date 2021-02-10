/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _COREBOOT_SRC_MAINBOARD_GOOGLE_TROGDOR_BOARD_H_
#define _COREBOOT_SRC_MAINBOARD_GOOGLE_TROGDOR_BOARD_H_

#include <assert.h>
#include <boardid.h>
#include <gpio.h>
#include <soc/gpio.h>

#define GPIO_EC_IN_RW	GPIO(118)
#define GPIO_AP_EC_INT	GPIO(94)
#define GPIO_H1_AP_INT	(CONFIG(TROGDOR_REV0) ? GPIO(21) : GPIO(42))
#define GPIO_SD_CD_L	GPIO(69)
#define GPIO_AMP_ENABLE	GPIO(23)

/* Display specific GPIOS */
#define GPIO_BACKLIGHT_ENABLE  GPIO(12)
#define GPIO_EDP_BRIDGE_ENABLE (CONFIG(TROGDOR_REV0) ? GPIO(14) : GPIO(104))
#define GPIO_EN_PP3300_DX_EDP  (CONFIG(TROGDOR_REV0) ? GPIO(106) : \
	(CONFIG(BOARD_GOOGLE_COACHZ) ? (board_id() == 0 ? GPIO(52) : \
	GPIO(67)) : GPIO(30)))

/* Fingerprint-specific GPIOs. Only for fingerprint-enabled devices (e.g. CoachZ). */
#if CONFIG(TROGDOR_HAS_FINGERPRINT)
#define GPIO_FPMCU_BOOT0	GPIO(10)
#define GPIO_FP_RST_L		GPIO(22)
#define GPIO_EN_FP_RAILS	GPIO(74)
#else
#define GPIO_FPMCU_BOOT0	dead_code_t(gpio_t)
#define GPIO_FP_RST_L		dead_code_t(gpio_t)
#define GPIO_EN_FP_RAILS	dead_code_t(gpio_t)
#endif


void setup_chromeos_gpios(void);

#endif /* _COREBOOT_SRC_MAINBOARD_GOOGLE_TROGDOR_BOARD_H_ */
