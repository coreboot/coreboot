/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <baseboard/variants.h>

static const struct soc_amd_gpio override_gpio_table[] = {
	/* Unused TP1056 */
	PAD_NC(GPIO_4),
	/* Unused TP1063 */
	PAD_NC(GPIO_17),
	PAD_NC(GPIO_18),
	/* LCD_PRIVACY_PCH */
	PAD_GPO(GPIO_5, HIGH),
};

static const struct soc_amd_gpio override_early_gpio_table[] = {
	PAD_NC(GPIO_18),
};

static const struct soc_amd_gpio override_pcie_gpio_table[] = {
	PAD_NC(GPIO_18),
};

const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(override_gpio_table);
	return override_gpio_table;
}

const struct soc_amd_gpio *variant_early_override_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(override_early_gpio_table);
	return override_early_gpio_table;
}

const struct soc_amd_gpio *variant_pcie_override_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(override_pcie_gpio_table);
	return override_pcie_gpio_table;
}
