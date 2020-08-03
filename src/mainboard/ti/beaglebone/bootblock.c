/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <bootblock_common.h>
#include <console/uart.h>
#include <soc/ti/am335x/clock.h>
#include <soc/ti/am335x/gpio.h>
#include <soc/ti/am335x/pinmux.h>

#include "leds.h"

void bootblock_mainboard_early_init(void)
{
	write32(&am335x_cm_wkup->wkup_gpio0, CM_ST_SW_WKUP | CM_FCLK_EN);
	write32(&am335x_cm_per->gpio1, CM_ST_SW_WKUP | CM_FCLK_EN);
	write32(&am335x_cm_per->gpio2, CM_ST_SW_WKUP | CM_FCLK_EN);
	write32(&am335x_cm_per->gpio3, CM_ST_SW_WKUP | CM_FCLK_EN);

	am335x_disable_gpio_irqs();

	beaglebone_leds_init();
	beaglebone_leds_set(BEAGLEBONE_LED_USR0, 1);
	beaglebone_leds_set(BEAGLEBONE_LED_USR1, 0);
	beaglebone_leds_set(BEAGLEBONE_LED_USR2, 1);
	beaglebone_leds_set(BEAGLEBONE_LED_USR3, 0);

	/* Set up the UART we're going to use */
	if (CONFIG_UART_FOR_CONSOLE == 0) {
		am335x_pinmux_uart0();
		write32(&am335x_cm_wkup->wkup_uart0, CM_ST_SW_WKUP);
	} else if (CONFIG_UART_FOR_CONSOLE == 1) {
		am335x_pinmux_uart1();
		write32(&am335x_cm_per->uart1, CM_ST_SW_WKUP);
	} else if (CONFIG_UART_FOR_CONSOLE == 2) {
		am335x_pinmux_uart2();
		write32(&am335x_cm_per->uart2, CM_ST_SW_WKUP);
	} else if (CONFIG_UART_FOR_CONSOLE == 3) {
		am335x_pinmux_uart3();
		write32(&am335x_cm_per->uart3, CM_ST_SW_WKUP);
	} else if (CONFIG_UART_FOR_CONSOLE == 4) {
		am335x_pinmux_uart4();
		write32(&am335x_cm_per->uart4, CM_ST_SW_WKUP);
	} else if (CONFIG_UART_FOR_CONSOLE == 5) {
		am335x_pinmux_uart5();
		write32(&am335x_cm_per->uart5, CM_ST_SW_WKUP);
	}

	/* Start monotonic timer */
	//rtc_start();
}
