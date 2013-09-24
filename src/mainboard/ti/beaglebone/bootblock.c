/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <types.h>
#include <uart.h>
#include <console/console.h>
#include <cpu/ti/am335x/clock.h>
#include <cpu/ti/am335x/gpio.h>
#include <cpu/ti/am335x/pinmux.h>

#include "leds.h"

void bootblock_mainboard_init(void);
void bootblock_mainboard_init(void)
{
	writel(CM_ST_SW_WKUP | CM_FCLK_EN, &am335x_cm_wkup->wkup_gpio0);
	writel(CM_ST_SW_WKUP | CM_FCLK_EN, &am335x_cm_per->gpio1);
	writel(CM_ST_SW_WKUP | CM_FCLK_EN, &am335x_cm_per->gpio2);
	writel(CM_ST_SW_WKUP | CM_FCLK_EN, &am335x_cm_per->gpio3);

	am335x_disable_gpio_irqs();

	beaglebone_leds_init();
	beaglebone_leds_set(BEAGLEBONE_LED_USR0, 1);
	beaglebone_leds_set(BEAGLEBONE_LED_USR1, 0);
	beaglebone_leds_set(BEAGLEBONE_LED_USR2, 1);
	beaglebone_leds_set(BEAGLEBONE_LED_USR3, 0);

	/* Set up the UART we're going to use */
	if (CONFIG_CONSOLE_SERIAL_UART0) {
		am335x_pinmux_uart0();
		writel(CM_ST_SW_WKUP, &am335x_cm_wkup->wkup_uart0);
	} else if (CONFIG_CONSOLE_SERIAL_UART1) {
		am335x_pinmux_uart1();
		writel(CM_ST_SW_WKUP, &am335x_cm_per->uart1);
	} else if (CONFIG_CONSOLE_SERIAL_UART2) {
		am335x_pinmux_uart2();
		writel(CM_ST_SW_WKUP, &am335x_cm_per->uart2);
	} else if (CONFIG_CONSOLE_SERIAL_UART3) {
		am335x_pinmux_uart3();
		writel(CM_ST_SW_WKUP, &am335x_cm_per->uart3);
	} else if (CONFIG_CONSOLE_SERIAL_UART4) {
		am335x_pinmux_uart4();
		writel(CM_ST_SW_WKUP, &am335x_cm_per->uart4);
	} else if (CONFIG_CONSOLE_SERIAL_UART5) {
		am335x_pinmux_uart5();
		writel(CM_ST_SW_WKUP, &am335x_cm_per->uart5);
	}

	/* Start monotonic timer */
	//rtc_start();
}
