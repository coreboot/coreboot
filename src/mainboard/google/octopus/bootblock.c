/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <ec/ec.h>
#include <soc/gpio.h>

void bootblock_mainboard_early_init(void)
{
	const struct pad_config *pads;
	size_t num;

	pads = mainboard_early_bootblock_gpio_table(&num);
	gpio_configure_pads(pads, num);
};

void bootblock_mainboard_init(void)
{
	const struct pad_config *pads, *override_pads;
	size_t num, override_num;

	/*
	 * Perform EC init before configuring GPIOs. This is because variant
	 * might talk to the EC to get board id and hence it will require EC
	 * init to have already performed.
	 */
	mainboard_ec_init();

	pads = variant_early_gpio_table(&num);
	override_pads = variant_early_override_gpio_table(&override_num);
	gpio_configure_pads_with_override(pads, num,
			override_pads, override_num);
}
