/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <mainboard/bootblock.h>

static const struct pad_config gpio_table[] = {
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),	/* UART0_RXD / UART_0_CRXD_DTXD */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),	/* UART0_TXD / UART_0_CTXD_DRXD */
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
