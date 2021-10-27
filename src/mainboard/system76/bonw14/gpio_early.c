/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_NF(GPP_C20, NONE, PLTRST, NF1), // UART2_RXD
	PAD_CFG_NF(GPP_C21, NONE, PLTRST, NF1), // UART2_TXD
	PAD_CFG_TERM_GPO(GPP_F22, 1, UP_20K, DEEP), // DGPU_RST#_PCH
	PAD_CFG_TERM_GPO(GPP_K22, 0, UP_5K, DEEP), // GPU_PWR_EN#
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
