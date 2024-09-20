/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <types.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}

static const struct cros_gpio cros_gpios[] = {
};

DECLARE_CROS_GPIOS(cros_gpios);

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}
