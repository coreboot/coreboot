/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <fw_config.h>
#include <ec/google/chromeec/ec.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* C12 : AP_PEN_DET_ODL has an external pull-up and hence no pad termination.*/
	PAD_CFG_GPI_GPIO_DRIVER(GPP_C12, NONE, DEEP),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
