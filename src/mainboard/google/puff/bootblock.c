/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <soc/gpio.h>

void bootblock_mainboard_early_init(void)
{
	const struct pad_config *variant_early_table;
	size_t variant_gpios;

	variant_early_table = variant_early_gpio_table(&variant_gpios);
	gpio_configure_pads(variant_early_table, variant_gpios);
}
