/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <soc/gpio.h>

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	PAD_CFG_GPI(GPP_F11, NONE, DEEP),
	PAD_CFG_GPI(GPP_F12, NONE, DEEP),
	PAD_CFG_GPI(GPP_F13, NONE, DEEP),
	PAD_CFG_GPI(GPP_F14, NONE, DEEP),
};

static const struct pad_config early_uart_gpio_table[] = {
	PAD_CFG_NF(GPP_F1, NONE, DEEP, NF2),	/* UART2_RXD */
	PAD_CFG_NF(GPP_F2, NONE, DEEP, NF2),	/* UART2_TXD */
};

void variant_configure_early_gpio_pads(void)
{
	if (CONFIG(INTEL_LPSS_UART_FOR_CONSOLE))
		gpio_configure_pads(early_uart_gpio_table, ARRAY_SIZE(early_uart_gpio_table));

	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
