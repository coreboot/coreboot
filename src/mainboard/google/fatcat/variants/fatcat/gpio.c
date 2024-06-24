/* SPDX-License-Identifier: GPL-2.0-or-later */

/* This header block is used to supply information to arbitrage, a
 * google-internal tool. Updating it incorrectly will lead to issues,
 * so please don't update it unless a change is specifically required.
 * BaseID: 3EC4CE58201758F4
 * Overrides: c826ba419f06f9df9cded8e60633253ddc7b60ff
 */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* TODO: Fill gpio configuration */
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* TODO: Fill gpio configuration */
};

static const struct pad_config romstage_gpio_table[] = {
	/* TODO: Fill gpio configuration */
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* Create the stub for romstage gpio, typically use for power sequence */
const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_PCH_WP, CROS_GPIO_DEVICE_NAME),
};

DECLARE_CROS_GPIOS(cros_gpios);
