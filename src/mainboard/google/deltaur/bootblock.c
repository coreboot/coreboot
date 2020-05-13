/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <ec/google/wilco/bootblock.h>
#include <soc/gpio.h>
#include <variant/gpio.h>

static void early_config_gpio(void)
{
	const struct pad_config *early_gpio_table;
	size_t num_gpios = 0;

	early_gpio_table = variant_early_gpio_table(&num_gpios);
	gpio_configure_pads(early_gpio_table, num_gpios);
}

void bootblock_mainboard_init(void)
{
	early_config_gpio();
	wilco_ec_early_init();
}
