/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <bootblock_common.h>

void bootblock_mainboard_early_init(void)
{
	const struct pad_config *pads;
	size_t num;
	pads = variant_early_gpio_table(&num);
	gpio_configure_pads(pads, num);
}

void bootblock_mainboard_init(void)
{
	variant_update_descriptor();
}

void __weak variant_update_descriptor(void) {}
