/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_GPO(GPP_A14, 0, DEEP), // DGPU_PWR_EN
	PAD_CFG_GPO(GPP_B2, 0, DEEP), // DGPU_RST#_PCH
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF1), // UART2_RXD (actually UART0)
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF1), // UART2_TXD (actually UART0)
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
