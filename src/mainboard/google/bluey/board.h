/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GOOGLE_BLUEY_BOARD_H
#define MAINBOARD_GOOGLE_BLUEY_BOARD_H

#include <assert.h>
#include <commonlib/coreboot_tables.h>
#include <gpio.h>

#define GPIO_AP_EC_INT		GPIO(67)
#define GSC_AP_INT(x)		GPIO(x)
#define GPIO_GSC_AP_INT		GSC_AP_INT(CONFIG_MAINBOARD_GPIO_PIN_FOR_GSC_AP_INTERRUPT)

#define TP_POWER_GPIO(x)	GPIO(x)
#define GPIO_TP_POWER_EN	TP_POWER_GPIO(CONFIG_MAINBOARD_GPIO_PIN_FOR_TOUCHPAD_POWER)

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

/* Display specific GPIOs */
#define GPIO_PANEL_POWER_ON	GPIO(70)
#define GPIO_PANEL_HPD		GPIO(119)

/* Touchscreen specific GPIOs */
#define TS_POWER_GPIO(x)	GPIO(x)
#define GPIO_TS_POWER_EN	TS_POWER_GPIO(CONFIG_MAINBOARD_GPIO_PIN_FOR_TOUCHSCREEN_POWER)

/* Charging GPIOs */
#define GPIO_PARALLEL_CHARGING_CFG GPIO(71)

#define GPIO_LID_OPEN_S3	GPIO(92)

/* SD card specific GPIOs. Only for SD-enabled devices. */
#if CONFIG(MAINBOARD_HAS_SD_CONTROLLER)
#define GPIO_SD_CD_L		GPIO(71)
#endif

/* USB Camera specific GPIOs */
#define GPIO_USB_CAM_RESET_L	GPIO(10)
#define GPIO_USB_CAM_ENABLE	GPIO(206)

/* USB-C1 port specific GPIOs */
#define GPIO_USB_C1_EN_PP3300		GPIO(186)
#define GPIO_USB_C1_EN_PP1800		GPIO(175)
#define GPIO_USB_C1_EN_PP0900		GPIO(188)
#define GPIO_USB_C1_RETIMER_RESET_L	GPIO(176)

#define GPIO_USB_C0_EN_L	GPIO(108)
#define GPIO_USB_C1_EN_L	GPIO(180)

/* GPIO for controlling the panel backlight */
#define BACKLIGHT_CONTROL_PMIC_GPIO 4
#define BACKLIGHT_CONTROL_PMIC_ID PMIC_D_SLAVE_ID

/* GPIO for controlling the panel backlight PWM */
#define BACKLIGHT_PWM_PMIC_GPIO 5
#define BACKLIGHT_PWM_PMIC_ID PMIC_A_SLAVE_ID

#define DEAD_BATT_CHG_THRESHOLD_MAH 100

void setup_chromeos_gpios(void);
bool is_off_mode(void);
void configure_parallel_charging(void);
void configure_parallel_charging_late(void);
void configure_debug_access_port(void);
void enable_slow_battery_charging(void);
void disable_slow_battery_charging(void);
void launch_charger_applet(void);
bool platform_get_battery_soc_information(uint32_t *batt_pct);
void enable_fast_battery_charging(void);
void init_sdam_config(void);
void configure_dead_battery_boot(void);
void clear_pending_ec_events(void);
/* Detect an AC unplug event with optional software debouncing */
int detect_ac_unplug_event(bool debounce);

#endif /* MAINBOARD_GOOGLE_BLUEY_BOARD_H */
