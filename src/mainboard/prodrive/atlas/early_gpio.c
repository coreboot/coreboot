/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>

#include "gpio.h"

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* todo: gpio config */
};

static const struct pad_config early_uart_gpio_table[] = {
	/* todo: gpio config */
};

void configure_early_gpio_pads(void)
{
	if (CONFIG(INTEL_LPSS_UART_FOR_CONSOLE))
		gpio_configure_pads(early_uart_gpio_table, ARRAY_SIZE(early_uart_gpio_table));

	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
