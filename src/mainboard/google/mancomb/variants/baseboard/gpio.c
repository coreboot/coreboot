/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

/* GPIO configuration in ramstage*/
static const struct soc_amd_gpio base_gpio_table[] = {
	/* TODO: Fill gpio configuration */
};

/* Early GPIO configuration in bootblock */
static const struct soc_amd_gpio bootblock_gpio_table[] = {
	/* TODO: Fill bootblock gpio configuration */
};

const struct soc_amd_gpio *__weak variant_base_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(base_gpio_table);
	return base_gpio_table;
}
const struct soc_amd_gpio *__weak variant_override_gpio_table(size_t *size)
{
	*size = 0;
	return NULL;
}

const struct soc_amd_gpio *__weak variant_bootblock_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(bootblock_gpio_table);
	return bootblock_gpio_table;
}
