/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <soc/gpio.h>

#include "gpio.h"

void bootblock_mainboard_early_init(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
