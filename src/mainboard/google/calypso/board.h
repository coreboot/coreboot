/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GOOGLE_MENSA_BOARD_H
#define MAINBOARD_GOOGLE_MENSA_BOARD_H

#include <assert.h>
#include <gpio.h>

/* TODO: update as per datasheet */
#define GPIO_AP_EC_INT		GPIO(67)
#define GSC_AP_INT(x)		GPIO(x)
#define GPIO_GSC_AP_INT		GSC_AP_INT(CONFIG_MAINBOARD_GPIO_PIN_FOR_GSC_AP_INTERRUPT)

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

#define DEAD_BATT_CHG_THRESHOLD_MAH 100

void setup_chromeos_gpios(void);
void configure_debug_access_port(void);
void enable_slow_battery_charging(void);
void disable_slow_battery_charging(void);
void launch_charger_applet(void);
bool platform_get_battery_soc_information(uint32_t *batt_pct);
void enable_fast_battery_charging(void);
void configure_dead_battery_boot(void);

#endif /* MAINBOARD_GOOGLE_MENSA_BOARD_H */
