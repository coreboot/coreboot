/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_KUKUI_GPIO_H__
#define __MAINBOARD_GOOGLE_KUKUI_GPIO_H__

#include <soc/gpio.h>

#define GPIO_WP		GPIO(PERIPHERAL_EN0)
#define EC_IRQ		GPIO(PERIPHERAL_EN1)
#define EC_IN_RW	GPIO(PERIPHERAL_EN14)
#define CR50_IRQ	GPIO(PERIPHERAL_EN3)
#define GPIO_RESET	GPIO(PERIPHERAL_EN8)
#define GPIO_EN_SPK_AMP	GPIO(PERIPHERAL_EN12)

void setup_chromeos_gpios(void);

#endif
