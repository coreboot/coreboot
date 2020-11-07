/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* WWAN_RST# */
	PAD_CFG_GPO(GPP_E10, 0, PLTRST),
	/* WWAN_PWR_EN */
	PAD_CFG_GPO(GPP_E13, 1, DEEP),
};

void variant_configure_early_gpio_pads(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
