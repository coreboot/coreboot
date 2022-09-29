/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include <variant/gpio.h>

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1), // UART2_RXD
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1), // UART2_TXD
	PAD_CFG_GPO(GPP_F8, 0, DEEP), // DGPU_RST#_PCH
	PAD_CFG_GPO(GPP_F9, 0, DEEP), // DGPU_PWR_EN
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1), // NB_ENAVDD
};

void variant_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
