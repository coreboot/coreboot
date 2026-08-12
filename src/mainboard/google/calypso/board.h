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
#define GPIO_EN_FP_RAILS	GPIO(22)
#if CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI)
#define GPIO_FPMCU_BOOT0	GPIO(24)
#define GPIO_FPMCU_INT		GPIO(23)
#else
#define GPIO_FPMCU_BOOT0	dead_code_t(gpio_t)
#define GPIO_FPMCU_INT		dead_code_t(gpio_t)
#endif
#endif

/* Soundwire AMP GPIOs */
#define GPIO_SNDW_0_SCL GPIO(202)
#define GPIO_FUN_SNDW_0_SCL 4	/* 4=LPASS_10:WSA_SWR_CLK, Also try 1=ENTERPRISEMGMT_11 */
#define GPIO_SNDW_0_SDA GPIO(203)
#define GPIO_FUN_SNDW_0_SDA 3	/* 3=LPASS_11:WSA_SWR_DATA */
#define GPIO_SNDW_AMP_0_ENABLE	GPIO(204)
#define GPIO_SNDW_AMP_1_ENABLE	GPIO(205)

#define DEAD_BATT_CHG_THRESHOLD_MAH 100

void setup_chromeos_gpios(void);
void configure_debug_access_port(void);
void enable_slow_battery_charging(void);
void disable_slow_battery_charging(void);
void launch_charger_applet(void);
bool platform_get_battery_soc_information(uint32_t *batt_pct);
void enable_fast_battery_charging(void);
void init_sdam_config(void);
void configure_dead_battery_boot(void);
bool mainboard_nvme_present(void);
void clear_pending_ec_events(void);
/*
 * Detect an AC unplug event
 *
 * @return 1 if AC is unplugged, 0 otherwise.
 */
int detect_ac_unplug_event(void);
/*
 * Signals the Chrome EC to register the final off-mode heartbeat
 * and initiates the AP power-off sequence.
 *
 * Input: bool skip_heartbeat - if true then wake immediately after shutdown
 *                              depending upon charger attached state.
 */
void chromeec_finalize_and_poweroff(bool skip_heartbeat);

#endif /* MAINBOARD_GOOGLE_MENSA_BOARD_H */
