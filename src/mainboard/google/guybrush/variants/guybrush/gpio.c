/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <gpio.h>
#include <soc/gpio.h>

/* This table is used by guybrush variant with board version < 2. */
static const struct soc_amd_gpio bid1_gpio_table[] = {
	/* EN_SPKR */
	PAD_GPO(GPIO_69, LOW),
};

const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	uint32_t board_version = board_id();
	*size = 0;

	if (board_version < 2) {
		*size = ARRAY_SIZE(bid1_gpio_table);
		return bid1_gpio_table;
	}

	return NULL;
}
