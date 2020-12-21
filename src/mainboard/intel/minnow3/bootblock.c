/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/gpio.h>

#include "gpio.h"

void bootblock_mainboard_early_init(void)
{
	const struct pad_config *pads;
	size_t num;

	pads = early_gpio_table(&num);
	gpio_configure_pads(pads, num);
}
