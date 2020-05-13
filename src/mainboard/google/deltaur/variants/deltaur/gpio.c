/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <gpio.h>
#include <variant/variant.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {

};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {

};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* Check if the device has a 360 sensor board present */
int has_360_sensor_board(void)
{
	return gpio_get(SENSOR_DET_360) == 0;
}
