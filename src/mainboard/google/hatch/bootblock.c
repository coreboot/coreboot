/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <soc/gpio.h>

static void early_config_gpio(void)
{
	const struct pad_config *variant_early_table;
	size_t variant_gpios;

	variant_early_table = variant_early_gpio_table(&variant_gpios);
	gpio_configure_pads(variant_early_table, variant_gpios);
}

void bootblock_mainboard_init(void)
{
	early_config_gpio();
}
