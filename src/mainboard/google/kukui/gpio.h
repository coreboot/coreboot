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
