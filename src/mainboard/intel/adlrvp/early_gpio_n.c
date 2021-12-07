/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <soc/gpio.h>

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* WWAN_RST# */
	PAD_CFG_GPO(GPP_F14, 0, PLTRST),
	/* WWAN_PWR_EN */
	PAD_CFG_GPO(GPP_D17, 1, DEEP),
	/* EC_IN_RW */
	PAD_CFG_GPI(GPP_E7, NONE, DEEP),
};

static const struct pad_config early_uart_gpio_table[] = {
	/* UART0 RX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* UART0 TX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
};

void variant_configure_early_gpio_pads(void)
{
	if (CONFIG(INTEL_LPSS_UART_FOR_CONSOLE))
		gpio_configure_pads(early_uart_gpio_table, ARRAY_SIZE(early_uart_gpio_table));

	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
