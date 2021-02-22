/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>

/* Pad configuration in ramstage*/
static const struct pad_config gpio_table[] = {
	/* D15 : EN_PP3300_CAMERA */
	PAD_CFG_GPO(GPP_D15, 1, PLTRST),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
