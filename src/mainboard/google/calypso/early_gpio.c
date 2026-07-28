/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/helpers.h>
#include <gpio.h>
#include "board.h"

enum gpio_state {
	GPIO_STATE_INPUT,
	GPIO_STATE_OUTPUT_LOW,
	GPIO_STATE_OUTPUT_HIGH,
};

struct gpio_config {
	gpio_t pad;
	enum gpio_state state;
};

#define GPIO_CFG_IN(pin_)       { .pad = GPIO(pin_), .state = GPIO_STATE_INPUT }
#define GPIO_CFG_OUT_LOW(pin_)  { .pad = GPIO(pin_), .state = GPIO_STATE_OUTPUT_LOW }
#define GPIO_CFG_OUT_HIGH(pin_) { .pad = GPIO(pin_), .state = GPIO_STATE_OUTPUT_HIGH }

static const struct gpio_config default_gpio_table[] = {
	/*
	 * INPUTS: Interrupts, Wakes, Detects, Faults, Open-Drain signals
	 * These are driven by external devices and have external pull-ups.
	 * Configured as inputs with No Pull (NP) to prevent leakage and voltage dividers.
	 */
	GPIO_CFG_IN(GPIO_SOC_TCHPAD_REPORT_EN_L),
	GPIO_CFG_IN(GPIO_SOC_TCHPAD_INT_ODL),
	GPIO_CFG_IN(GPIO_SOC_WP_OD),
	GPIO_CFG_IN(GPIO_SAR0_INT_ODL),
	GPIO_CFG_IN(GPIO_SAR1_INT_ODL),
	GPIO_CFG_IN(GPIO_TCHSCR_INT_ODL),
	GPIO_CFG_IN(GPIO_EC_SOC_QSH_INT_ODL),
	GPIO_CFG_IN(GPIO_OOB_OPQ_INT_N),
	GPIO_CFG_IN(GPIO_LID_OPEN_S3),
	GPIO_CFG_IN(GPIO_SOC_ALS_INT_ODL),
	GPIO_CFG_IN(GPIO_USBA_FAULT_ODL),
	GPIO_CFG_IN(GPIO_WAKE_ON_WWAN_N),
	GPIO_CFG_IN(GPIO_PCIE_4_WLAN_WAKE_L),
	GPIO_CFG_IN(GPIO_PCIE_6_WAKE_N_1P2),
	GPIO_CFG_IN(GPIO_PCIE_5_SSD_WAKE_ODL),
	GPIO_CFG_IN(GPIO_PCIE_3_WWAN_WAKE_1P2_L),
	GPIO_CFG_IN(GPIO_SD_CARD_DET_ODL),
	GPIO_CFG_IN(GPIO_SMB2370_CHG_LED_PG_Q_ODL),

	/*
	 * OUTPUTS: Resets and Disables
	 * These are active-low signals. Driving them low (0) ensures
	 * the connected peripherals are held in reset/disabled during boot.
	 */
	GPIO_CFG_OUT_LOW(GPIO_TCHSCR_RST_L),
};

void configure_gpios_defaults(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(default_gpio_table); i++) {
		const struct gpio_config *cfg = &default_gpio_table[i];

		switch (cfg->state) {
		case GPIO_STATE_INPUT:
			gpio_input(cfg->pad);
			break;
		case GPIO_STATE_OUTPUT_LOW:
			gpio_output(cfg->pad, 0);
			break;
		case GPIO_STATE_OUTPUT_HIGH:
			gpio_output(cfg->pad, 1);
			break;
		}
	}
}
