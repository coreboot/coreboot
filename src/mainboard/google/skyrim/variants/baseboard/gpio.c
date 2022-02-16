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

__weak void variant_base_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(base_gpio_table);
	*gpio = base_gpio_table;
}

__weak void variant_override_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = 0;
	*gpio = NULL;
}

__weak void variant_bootblock_gpio_table(const struct soc_amd_gpio **gpio, size_t *size)
{
	*size = ARRAY_SIZE(bootblock_gpio_table);
	*gpio = bootblock_gpio_table;
}
