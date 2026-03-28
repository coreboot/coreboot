/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <mainboard/bootblock.h>

static const struct pad_config gpio_table[] = {
	/* UART0 for console */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),	/* UART0_RXD */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),	/* UART0_TXD */

	/* SMBus for DDR5 SPD */
	PAD_CFG_NF(GPP_C00, NONE, DEEP, NF1),	/* SMBCLK */
	PAD_CFG_NF(GPP_C01, NONE, DEEP, NF1),	/* SMBDATA */
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
