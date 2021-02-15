/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* WWAN_RST# */
	PAD_CFG_GPO(GPP_F14, 0, PLTRST),
	/* WWAN_PWR_EN */
	PAD_CFG_GPO(GPP_F21, 1, DEEP),
	/* SMB_CLK */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	/* SMB_DATA */
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
};

void variant_configure_early_gpio_pads(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
