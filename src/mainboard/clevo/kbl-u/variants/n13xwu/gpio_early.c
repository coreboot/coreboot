/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include <variant/gpio.h>

static const struct pad_config early_gpio_table[] = {
	// UART2_RXD
	_PAD_CFG_STRUCT(GPP_C20,	0x44000702,	0x0),

	// UART2_TXD
	_PAD_CFG_STRUCT(GPP_C21,	0x44000700,	0x0)
};

void variant_configure_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
