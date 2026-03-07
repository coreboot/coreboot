/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

/* Configure SMBus pins before FSP-M reads SPD for memory training. */
static const struct pad_config early_gpio_table[] = {
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),	/* SMBCLK */
	PAD_CFG_NF(GPP_C1, DN_20K, DEEP, NF1),	/* SMBDATA */
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
