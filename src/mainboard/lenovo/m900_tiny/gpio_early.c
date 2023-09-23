/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

/* Early pad configuration */
static const struct pad_config uart_gpio_table[] = {
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1), /* UART2_RXD */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1), /* UART2_TXD */
};

void mainboard_configure_early_gpios(void)
{
	if (CONFIG(INTEL_LPSS_UART_FOR_CONSOLE))
		gpio_configure_pads(uart_gpio_table, ARRAY_SIZE(uart_gpio_table));
}
