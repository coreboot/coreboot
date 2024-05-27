/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config early_gpio_table[] = {
	/* GPP_C00 - SMBCLK */
	PAD_CFG_NF(GPP_C00, UP_20K, DEEP, NF1),
	/* GPP_C01 - SMBDATA */
	PAD_CFG_NF(GPP_C01, UP_20K, DEEP, NF1),
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
