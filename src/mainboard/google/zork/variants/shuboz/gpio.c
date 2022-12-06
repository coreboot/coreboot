/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <ec/google/chromeec/ec.h>

static const struct soc_amd_gpio bid_gpio_set_stage_ram[] = {
	/* PEN_DETECT_ODL - Not connected */
	PAD_NC(GPIO_4),
	/* AGPIO_5 - NC */
	PAD_NC(GPIO_5),
	/* EGPIO141 - NC */
	PAD_NC(GPIO_141),
	/* EGPIO144 - NC (etk5515 not used) */
	PAD_NC(GPIO_144),
};

const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(bid_gpio_set_stage_ram);
	return bid_gpio_set_stage_ram;
}
