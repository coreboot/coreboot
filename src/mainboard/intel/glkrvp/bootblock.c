/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <ec/ec.h>
#include <intelblocks/lpc_lib.h>
#include <soc/gpio.h>
#include <variant/ec.h>

void bootblock_mainboard_init(void)
{
	const struct pad_config *pads;
	size_t num;

	lpc_configure_pads();
	pads = variant_early_gpio_table(&num);
	gpio_configure_pads(pads, num);
	mainboard_ec_init();
}
