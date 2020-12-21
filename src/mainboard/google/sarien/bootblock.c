/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <ec/google/wilco/bootblock.h>
#include <soc/gpio.h>
#include <variant/gpio.h>

void bootblock_mainboard_early_init(void)
{
	const struct pad_config *early_gpio_table;
	size_t num_gpios = 0;

	early_gpio_table = variant_early_gpio_table(&num_gpios);
	gpio_configure_pads(early_gpio_table, num_gpios);
}

void bootblock_mainboard_init(void)
{
	wilco_ec_early_init();
}
