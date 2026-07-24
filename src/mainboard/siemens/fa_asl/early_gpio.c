/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <soc/gpio.h>

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {

};

static const struct pad_config early_uart_gpio_table[] = {
	/* UART1 RX */
	PAD_CFG_NF(GPP_D17, UP_20K, DEEP, NF1),
	/* UART1 TX */
	PAD_CFG_NF(GPP_D18, UP_20K, DEEP, NF1),
};

void variant_configure_early_gpio_pads(void)
{
	if (CONFIG(INTEL_LPSS_UART_FOR_CONSOLE))
		gpio_configure_pads(early_uart_gpio_table, ARRAY_SIZE(early_uart_gpio_table));

	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
