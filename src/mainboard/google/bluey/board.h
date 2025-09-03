/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GOOGLE_BLUEY_BOARD_H
#define MAINBOARD_GOOGLE_BLUEY_BOARD_H

#include <assert.h>
#include <gpio.h>

#define GPIO_AP_EC_INT		GPIO(67)
#define GSC_AP_INT(x)		GPIO(x)
#define GPIO_GSC_AP_INT		GSC_AP_INT(CONFIG_MAINBOARD_GPIO_PIN_FOR_GSC_AP_INTERRUPT)

#define TP_POWER_GPIO(x)	GPIO(x)
#define GPIO_TP_POWER_EN	TP_POWER_GPIO(CONFIG_MAINBOARD_GPIO_PIN_FOR_TOUCHPAD_POWER)

/* Fingerprint-specific GPIOs. Only for fingerprint-enabled devices. */
#if CONFIG(MAINBOARD_HAS_FINGERPRINT)
#define GPIO_FP_RST_L		GPIO(25)
#if CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI)
#define GPIO_FPMCU_BOOT0	GPIO(24)
#define GPIO_FPMCU_INT		GPIO(23)
#define GPIO_EN_FP_RAILS	GPIO(22)
#else
#define GPIO_FPMCU_BOOT0	dead_code_t(gpio_t)
#define GPIO_FPMCU_INT		dead_code_t(gpio_t)
#define GPIO_EN_FP_RAILS	dead_code_t(gpio_t)
#endif
#endif

/* Soundwire AMP GPIOs */
#define GPIO_SNDW_AMP_0_ENABLE	GPIO(204)
#define GPIO_SNDW_AMP_1_ENABLE	GPIO(205)

/* Display specific GPIOs */
#define GPIO_PANEL_POWER_ON	GPIO(70)
#define GPIO_PANEL_HPD		GPIO(119)

/* SD card specific GPIOs. Only for SD-enabled devices. */
#if CONFIG(MAINBOARD_HAS_SD_CONTROLLER)
#define GPIO_SD_CD_L		GPIO(71)
#endif

void setup_chromeos_gpios(void);
void enable_slow_battery_charging(void);
void disable_slow_battery_charging(void);

#endif /* MAINBOARD_GOOGLE_BLUEY_BOARD_H */
