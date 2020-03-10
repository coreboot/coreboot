/*
 * This file is part of the coreboot project.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {

};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {

};

const struct pad_config *__weak variant_base_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *__weak variant_override_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}

const struct pad_config *__weak variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {

};

const struct cros_gpio *__weak variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}
