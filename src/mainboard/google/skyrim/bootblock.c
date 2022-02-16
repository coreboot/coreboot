/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <baseboard/variants.h>

void bootblock_mainboard_early_init(void)
{
	/* TODO: Perform mainboard initialization */
}

void bootblock_mainboard_init(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	variant_bootblock_gpio_table(&gpios, &num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}
