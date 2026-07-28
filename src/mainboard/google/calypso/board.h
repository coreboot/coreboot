/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GOOGLE_CALYPSO_BOARD_H
#define MAINBOARD_GOOGLE_CALYPSO_BOARD_H

#include <assert.h>
#include <gpio.h>

/* GPIO Pin Definitions */
#define GPIO_SOC_TCHPAD_REPORT_EN_L	2
#define GPIO_SOC_TCHPAD_INT_ODL		3
#define GPIO_SOC_WP_OD			27
#define GPIO_SAR0_INT_ODL		30
#define GPIO_SAR1_INT_ODL		31
#define GPIO_TCHSCR_RST_L		48
#define GPIO_TCHSCR_INT_ODL		51
#define GPIO_EC_SOC_QSH_INT_ODL		66
#define GPIO_OOB_OPQ_INT_N		71
#define GPIO_LID_OPEN_S3		92
#define GPIO_SOC_ALS_INT_ODL		93
#define GPIO_USBA_FAULT_ODL		105
#define GPIO_WAKE_ON_WWAN_N		141
#define GPIO_PCIE_4_WLAN_WAKE_L		148
#define GPIO_PCIE_6_WAKE_N_1P2		151
#define GPIO_PCIE_5_SSD_WAKE_ODL	154
#define GPIO_PCIE_3_WWAN_WAKE_1P2_L	157
#define GPIO_SD_CARD_DET_ODL		221
#define GPIO_SMB2370_CHG_LED_PG_Q_ODL	245

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
void configure_gpios_defaults(void);

#endif /* MAINBOARD_GOOGLE_CALYPSO_BOARD_H */
