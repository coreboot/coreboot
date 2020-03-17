/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <ec/ec.h>
#include <intelblocks/lpc_lib.h>
#include <soc/gpio.h>

void bootblock_mainboard_init(void)
{
	const struct pad_config *pads, *override_pads;
	size_t num, override_num;

	lpc_configure_pads();

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
