/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <types.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
};

const struct pad_config *__weak variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *__weak variant_gpio_override_table(size_t *num)
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
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_PCH_WP, CROS_GPIO_DEVICE_NAME),
};

DECLARE_WEAK_CROS_GPIOS(cros_gpios);

const struct pad_config *__weak variant_romstage_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}
