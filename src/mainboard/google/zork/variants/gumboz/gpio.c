/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>

static const struct soc_amd_gpio dirinboz_gpio_set_stage_ram[] = {
	/* PEN_DETECT_ODL - no used */
	PAD_NC(GPIO_4),
	/* PEN_POWER_EN - no used */
	PAD_NC(GPIO_5),
};

const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(dirinboz_gpio_set_stage_ram);
	return dirinboz_gpio_set_stage_ram;
}
