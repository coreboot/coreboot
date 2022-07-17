/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1), // UART2_RXD
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1), // UART2_TXD
	PAD_CFG_GPO(GPP_U4, 0, DEEP), // DGPU_RST#_PCH
	PAD_CFG_GPO(GPP_U5, 0, DEEP), // DGPU_PWR_EN
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
