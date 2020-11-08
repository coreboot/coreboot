/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config early_gpio_table[] = {
	/* Early LPC configuration in romstage */
	PAD_CFG_NF(GPP_A1, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_A2, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_A3, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_A4, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_A9, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_A10, NONE, DEEP, NF1),

	/* BMC POST_COMPLETE */
	PAD_CFG_GPO(GPP_B20, 0, PLTRST),
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
